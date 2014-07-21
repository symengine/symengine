#include "matrix.h"
#include "add.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {
// ----------------------------- CSRMatrix ------------------------------------
CSRMatrix::CSRMatrix(unsigned row, unsigned col)
        : MatrixBase(row, col)
{
}

CSRMatrix::CSRMatrix(unsigned row, unsigned col, std::vector<unsigned>&& p,
    std::vector<unsigned>&& j, std::vector<RCP<const Basic>>&& x)
        : MatrixBase(row, col), p_{std::move(p)}, j_{std::move(j)}, x_{std::move(x)}
{
    CSYMPY_ASSERT(is_canonical());
}

bool CSRMatrix::is_canonical()
{
	if (p_.size() != row_ + 1 || j_.size() != p_[row_] || x_.size() != p_[row_])
		return false;
	// Check if column indices are strictly increasing so we know for sure that
	// they are sorted and no duplicates
    for (unsigned i = 0; i < row_; i++) {
        if (p_[i] > p_[i + 1])
            return false;
        for (unsigned jj = p_[i] + 1; jj < p_[i + 1]; jj++) {
            if (!(j_[jj - 1] < j_[jj]) ) {
                return false;
            }
        }
    }
    return true;
}

// Get and set elements
RCP<const Basic> CSRMatrix::get(unsigned i) const
{
    unsigned row = i / col_;
    unsigned col = i - row*col_;

    unsigned row_start = p_[row];
    unsigned row_end = p_[row + 1] - 1;

    for (unsigned j = row_start; j <= row_end; j++) {
        if (j_[j] == col)
            return x_[j];
        else if (j_[j] > col)
            break;
    }

    return zero;
}

void CSRMatrix::set(unsigned i, RCP<const Basic> &e)
{
}

unsigned CSRMatrix::rank() const
{
    throw std::runtime_error("Not implemented.");
}

RCP<const Basic> CSRMatrix::det() const
{
    throw std::runtime_error("Not implemented.");
}

RCP<const MatrixBase> CSRMatrix::inv() const
{
    throw std::runtime_error("Not implemented.");
}

MatrixBase& CSRMatrix::add_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

MatrixBase& CSRMatrix::mul_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

CSRMatrix CSRMatrix::from_coo(unsigned row, unsigned col,
    const std::vector<unsigned>& i, const std::vector<unsigned>& j,
    const std::vector<RCP<const Basic>>& x)
{
    unsigned nnz = x.size();
    std::vector<unsigned> p_ = std::vector<unsigned>(row + 1);
    std::vector<unsigned> j_ = std::vector<unsigned>(nnz);
    std::vector<RCP<const Basic>> x_ = std::vector<RCP<const Basic>>(nnz);

    for (unsigned n = 0; n < nnz; n++) {
        p_[i[n]]++;
    }

    // cumsum the nnz per row to get p
    for (unsigned i = 0, cumsum = 0; i < row; i++) {
        unsigned temp = p_[i];
        p_[i] = cumsum;
        cumsum += temp;
    }
    p_[row] = nnz;

    // write j, x into j_, x_
    for (unsigned n = 0; n < nnz; n++) {
        unsigned row  = i[n];
        unsigned dest = p_[row];

        j_[dest] = j[n];
        x_[dest] = x[n];

        p_[row]++;
    }

    for (unsigned i = 0, last = 0; i <= row; i++) {
        unsigned temp = p_[i];
        p_[i]  = last;
        last   = temp;
    }

    CSRMatrix B = CSRMatrix(row, col, std::move(p_), std::move(j_), std::move(x_));
    return B;
}

// ----------------------- Additional methods --------------------------------//
void csr_sum_duplicates(CSRMatrix &A)
{
    unsigned nnz = 0;
    unsigned row_end = 0;
    for (unsigned i = 0; i < A.row_; i++) {
        unsigned jj = row_end;
        row_end = A.p_[i + 1];
        while (jj < row_end) {
            unsigned j = A.j_[jj];
            RCP<const Basic> x = A.x_[jj];
            jj++;

            while (jj < row_end && A.j_[jj] == j) {
                x = add(x, A.x_[jj]);
                jj++;
            }
            A.j_[nnz] = j;
            A.x_[nnz] = x;
            nnz++;
        }
        A.p_[i + 1] = nnz;
    }
}

bool csr_has_sorted_indices(const CSRMatrix &A)
{
    for (unsigned i = 0; i < A.row_; i++) {
        for (unsigned jj = A.p_[i]; jj < A.p_[i + 1] - 1; jj++) {
            if (A.j_[jj] > A.j_[jj + 1]) {
                return false;
            }
        }
    }
    return true;
}

bool csr_has_canonical_format(const CSRMatrix &A)
{
    for (unsigned i = 0; i < A.row_; i++) {
        if (A.p_[i] > A.p_[i + 1])
            return false;
        for (unsigned jj = A.p_[i] + 1; jj < A.p_[i + 1]; jj++) {
            if (!(A.j_[jj - 1] < A.j_[jj]) ) {
                return false;
            }
        }
    }
    return true;
}

template< class T1, class T2 >
bool kv_pair_less(const std::pair<T1,T2>& x, const std::pair<T1,T2>& y){
    return x.first < y.first;
}

void csr_sort_indices(CSRMatrix &A)
{
    std::vector< std::pair<unsigned, RCP<const Basic>> > temp;

    for (unsigned i = 0; i < A.row_; i++) {
        unsigned row_start = A.p_[i];
        unsigned row_end   = A.p_[i + 1];

        temp.clear();

        for(unsigned jj = row_start; jj < row_end; jj++){
            temp.push_back(std::make_pair(A.j_[jj],A.x_[jj]));
        }

        std::sort(temp.begin(),temp.end(), kv_pair_less<unsigned, RCP<const Basic>>);

        for(unsigned jj = row_start, n = 0; jj < row_end; jj++, n++){
            A.j_[jj] = temp[n].first;
            A.x_[jj] = temp[n].second;
        }
    }
}

void csr_matmat_pass1(const CSRMatrix &A, const CSRMatrix &B, CSRMatrix &C)
{
    // method that uses O(n) temp storage
    std::vector<unsigned> mask(A.col_, -1);
    C.p_[0] = 0;

    unsigned nnz = 0;
    for (unsigned i = 0; i < A.row_; i++) {
        // npy_intp row_nnz = 0;
        unsigned row_nnz = 0;

        for (unsigned jj = A.p_[i]; jj < A.p_[i + 1]; jj++) {
            unsigned j = A.j_[jj];
            for (unsigned kk = B.p_[j]; kk < B.p_[j + 1]; kk++) {
                unsigned k = B.j_[kk];
                if(mask[k] != i){
                    mask[k] = i;
                    row_nnz++;
                }
            }
        }

        unsigned next_nnz = nnz + row_nnz;

        // if (row_nnz > NPY_MAX_INTP - nnz || next_nnz != (I)next_nnz) {

        //      * Index overflowed. Note that row_nnz <= n_col and cannot overflow

        //     throw std::overflow_error("nnz of the result is too large");
        // }

        nnz = next_nnz;
        C.p_[i + 1] = nnz;
    }
}


// Pass 2 computes CSR entries for matrix C = A*B using the
// row pointer Cp[] computed in Pass 1.
void csr_matmat_pass2(const CSRMatrix &A, const CSRMatrix &B, CSRMatrix &C)
{
    std::vector<int> next(A.col_, -1);
    std::vector<RCP<const Basic>> sums(A.col_, zero);

    unsigned nnz = 0;

    C.p_[0] = 0;

    for (unsigned i = 0; i < A.row_; i++) {
        int head   = -2;
        unsigned length =  0;

        unsigned jj_start = A.p_[i];
        unsigned jj_end   = A.p_[i + 1];
        for (unsigned jj = jj_start; jj < jj_end; jj++) {
            unsigned j = A.j_[jj];
            RCP<const Basic> v = A.x_[jj];

            unsigned kk_start = B.p_[j];
            unsigned kk_end   = B.p_[j+1];
            for (unsigned kk = kk_start; kk < kk_end; kk++) {
                unsigned k = B.j_[kk];

                sums[k] = add(sums[k], mul(v, B.x_[kk]));

                if (next[k] == -1) {
                    next[k] = head;
                    head  = k;
                    length++;
                }
            }
        }

        for (unsigned jj = 0; jj < length; jj++) {

            if (neq(sums[head], zero)) {
                C.j_[nnz] = head;
                C.x_[nnz] = sums[head];
                nnz++;
            }

            unsigned temp = head;
            head = next[head];

            next[temp] = -1; //clear arrays
            sums[temp] =  zero;
        }

        C.p_[i + 1] = nnz;
    }
}

} // CSymPy
