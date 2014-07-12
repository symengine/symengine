#include "matrix.h"

namespace CSymPy {

std::string MatrixBase::__str__() const {
    std::ostringstream o;

    for (unsigned i = 0; i < row_; i++) {
        o << "[";
        for (unsigned j = 0; j < col_ - 1; j++)
            o << *this->get(i*col_ + j) << ", ";
        o << *this->get(i*col_ + col_ - 1) << "]" << std::endl;
    }

    return o.str();
}

bool MatrixBase::eq(const MatrixBase &other) const
{
	if (this->nrows() != other.nrows() || this->ncols() != other.ncols())
        return false;

    for (unsigned i = 0; i < this->nrows()*this->ncols(); i++)
        if(neq(this->get(i), other.get(i)))
            return false;

    return true;
}

} // CSymPy
