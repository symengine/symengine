#ifndef SYMENGINE_POLYNOMIALS_MULTIVARIATE
#define SYMENGINE_POLYNOMIALS_MULTIVARIATE

#include <symengine/expression.h>
#include <symengine/monomials.h>

namespace SymEngine
{

class MultivariateIntPolynomial : public Basic
{
public:
    // vars: set of variables for the polynomial
    // degrees: max degrees of the symbols
    // dict: dictionary for sparse represntation of polynomial, x**1 * y**2 + 3
    // * x**4 * y ** 5
    // is represented as {(1,2):1,(4,5):3}
    set_sym vars_;
    umap_sym_uint degrees_;
    umap_uvec_mpz dict_;

private:
    // Creates a MultivariateIntPolynomial in cannonical form based on
    // dictionary d.
    static RCP<const MultivariateIntPolynomial> from_dict(const set_sym &s,
                                                          umap_uvec_mpz &&d);

public:
    IMPLEMENT_TYPEID(MULTIVARIATEINTPOLYNOMIAL)
    // constructor from components
    MultivariateIntPolynomial(const set_sym &var, umap_sym_uint &degrees,
                              umap_uvec_mpz &dict);
    // Public function for creating MultivariateIntPolynomial:
    // vec_sym can contain symbols for the polynomial in any order, but the
    // symbols
    // will be sorted in the set_sym of the actual object.
    // The order of the exponenents in the vectors in the dictionary will also
    // be permuted accordingly.
    static RCP<const MultivariateIntPolynomial>
    multivariate_int_polynomial(const vec_sym &v, umap_uvec_mpz &&d);

    vec_basic get_args() const;
    bool is_canonical(const set_sym &vars, const umap_sym_uint &degrees,
                      const umap_uvec_mpz &dict);
    std::size_t __hash__() const;
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;
    integer_class eval(std::map<RCP<const Symbol>, integer_class,
                                RCPSymbolCompare> &vals) const;

    friend RCP<const MultivariateIntPolynomial>
    add_mult_poly(const MultivariateIntPolynomial &a,
                  const MultivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    neg_mult_poly(const MultivariateIntPolynomial &a);
    friend RCP<const MultivariateIntPolynomial>
    sub_mult_poly(const MultivariateIntPolynomial &a,
                  const MultivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    mul_mult_poly(const MultivariateIntPolynomial &a,
                  const MultivariateIntPolynomial &b);

    friend RCP<const MultivariateIntPolynomial>
    add_mult_poly(const MultivariateIntPolynomial &a,
                  const UnivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    add_mult_poly(const UnivariateIntPolynomial &a,
                  const MultivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    sub_mult_poly(const MultivariateIntPolynomial &a,
                  const UnivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    sub_mult_poly(const UnivariateIntPolynomial &a,
                  const MultivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    mul_mult_poly(const MultivariateIntPolynomial &a,
                  const UnivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    mul_mult_poly(const UnivariateIntPolynomial &a,
                  const MultivariateIntPolynomial &b);

    friend RCP<const MultivariateIntPolynomial>
    add_mult_poly(const UnivariateIntPolynomial &a,
                  const UnivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    sub_mult_poly(const UnivariateIntPolynomial &a,
                  const UnivariateIntPolynomial &b);
    friend RCP<const MultivariateIntPolynomial>
    mul_mult_poly(const UnivariateIntPolynomial &a,
                  const UnivariateIntPolynomial &b);
};

// reconciles the positioning of the exponents in the vectors in the
// umap_uvec_mpz dict_ of the arguments
// with the positioning of the exponents in the correspondng vectors of the
// output of the function.
// v1 and v2 are vectors whose indices are the positions in the arguments and
// whose values are the
// positions in the output.  set_sym s is the set of symbols of the output, and
// s1 and s2 are the sets of the symbols of the inputs.
unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_sym &s,
                       const set_sym &s1, const set_sym &s2);
// same as above, but for reconciling representation of a UnivariatePolynomial.
unsigned int reconcile(vec_uint &v1, unsigned int &v2, set_sym &s,
                       const set_sym &s1, const RCP<const Symbol> s2);
// translates vectors of exponents from one polynomial into vectors of exponents
// for another.
// i.e. a polynomial if one of the addends is a polynomial in x,y and the sum is
// a polynomial
// in x,y,z, we will need to translate vectors with two elements to vectors with
// three elements.
// This situation can arise when we want to preform operations with two
// polynomials in different
// variables
// translator is the vector generated by the reconcile function and size is the
// size of the
// resulting vector.
vec_uint translate(vec_uint original, vec_uint translator, unsigned int size);
// translates terms of UnivariateIntPolynomial into vectors
vec_uint translate(unsigned int original, unsigned int translator,
                   unsigned int size);
// translates two vec_uints to the desired format and adds them together
// componentwise
vec_uint uint_vec_translate_and_add(const vec_uint &v1, const vec_uint &v2,
                                    const vec_uint &translator1,
                                    const vec_uint &translator2,
                                    const unsigned int size);
vec_uint uint_vec_translate_and_add(const vec_uint &v1, const unsigned int v2,
                                    const vec_uint &translator1,
                                    const unsigned int &translator2,
                                    const unsigned int size);

unsigned int reconcile(vec_int &v1, vec_int &v2, set_sym &s, const set_sym &s1,
                       const set_sym &s2);
unsigned int reconcile(vec_int &v1, unsigned int &v2, set_sym &s,
                       const set_sym &s1, const RCP<const Symbol> s2);

vec_int translate(vec_int original, vec_uint translator, unsigned int size);
vec_int translate(int original, unsigned int translator, unsigned int size);
vec_int int_vec_translate_and_add(const vec_int &v1, const vec_int &v2,
                                  const vec_uint &translator1,
                                  const vec_uint &translator2,
                                  const unsigned int size);
vec_int int_vec_translate_and_add(const vec_int &v1, const unsigned int v2,
                                  const vec_uint &translator1,
                                  const unsigned int &translator2,
                                  const unsigned int size);

RCP<const MultivariateIntPolynomial>
add_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
neg_mult_poly(const MultivariateIntPolynomial &a);
RCP<const MultivariateIntPolynomial>
sub_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
mul_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b);

RCP<const MultivariateIntPolynomial>
add_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
add_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
sub_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
sub_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
mul_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
mul_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b);

RCP<const MultivariateIntPolynomial>
add_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
sub_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b);
RCP<const MultivariateIntPolynomial>
mul_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b);

// MultivariatePolynomial
class MultivariatePolynomial : public Basic
{
public:
    // vars: set of variables for the polynomial
    // degrees: max degrees of the symbols
    // dict: dictionary for sparse represntation of polynomial, x**1 * y**2 + 3
    // * x**4 * y ** 5
    // is represented as {(1,2):1,(4,5):3}
    set_sym vars_;
    umap_sym_int degrees_;
    umap_vec_expr dict_;

private:
    // creates a MultivariatePolynomial in cannonical form based on dictionary
    // d.
    static RCP<const MultivariatePolynomial> from_dict(const set_sym &s,
                                                       umap_vec_expr &&d);

public:
    IMPLEMENT_TYPEID(MULTIVARIATEPOLYNOMIAL);
    // constructor from components
    MultivariatePolynomial(const set_sym &var, umap_sym_int &degrees,
                           umap_vec_expr &dict);
    // Public function for creating MultivariatePolynomial:
    // vec_sym can contain symbols for the polynomial in any order, but the
    // symbols
    // will be sorted in the set_sym of the actual object.
    // The order of the exponenents in the vectors in the dictionary will also
    // be permuted accordingly.
    static RCP<const MultivariatePolynomial>
    multivariate_polynomial(const vec_sym &v, umap_vec_expr &&d);

    vec_basic get_args() const;
    bool is_canonical(const set_sym &vars, const umap_sym_int &degrees,
                      const umap_vec_expr &dict);
    std::size_t __hash__() const;
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;
    Expression
    eval(std::map<RCP<const Symbol>, Expression, RCPSymbolCompare> &vals) const;

    friend RCP<const MultivariatePolynomial>
    add_mult_poly(const MultivariatePolynomial &a,
                  const MultivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    neg_mult_poly(const MultivariatePolynomial &a);
    friend RCP<const MultivariatePolynomial>
    sub_mult_poly(const MultivariatePolynomial &a,
                  const MultivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    mul_mult_poly(const MultivariatePolynomial &a,
                  const MultivariatePolynomial &b);

    friend RCP<const MultivariatePolynomial>
    add_mult_poly(const MultivariatePolynomial &a,
                  const UnivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    add_mult_poly(const UnivariatePolynomial &a,
                  const MultivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    sub_mult_poly(const MultivariatePolynomial &a,
                  const UnivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    sub_mult_poly(const UnivariatePolynomial &a,
                  const MultivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    mul_mult_poly(const MultivariatePolynomial &a,
                  const UnivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    mul_mult_poly(const UnivariatePolynomial &a,
                  const MultivariatePolynomial &b);

    friend RCP<const MultivariatePolynomial>
    add_mult_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    sub_mult_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
    friend RCP<const MultivariatePolynomial>
    mul_mult_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
};

RCP<const MultivariatePolynomial>
add_mult_poly(const MultivariatePolynomial &a, const MultivariatePolynomial &b);
RCP<const MultivariatePolynomial>
neg_mult_poly(const MultivariatePolynomial &a);
RCP<const MultivariatePolynomial>
sub_mult_poly(const MultivariatePolynomial &a, const MultivariatePolynomial &b);
RCP<const MultivariatePolynomial>
mul_mult_poly(const MultivariatePolynomial &a, const MultivariatePolynomial &b);

RCP<const MultivariatePolynomial> add_mult_poly(const MultivariatePolynomial &a,
                                                const UnivariatePolynomial &b);
RCP<const MultivariatePolynomial>
add_mult_poly(const UnivariatePolynomial &a, const MultivariatePolynomial &b);
RCP<const MultivariatePolynomial> sub_mult_poly(const MultivariatePolynomial &a,
                                                const UnivariatePolynomial &b);
RCP<const MultivariatePolynomial>
sub_mult_poly(const UnivariatePolynomial &a, const MultivariatePolynomial &b);
RCP<const MultivariatePolynomial> mul_mult_poly(const MultivariatePolynomial &a,
                                                const UnivariatePolynomial &b);
RCP<const MultivariatePolynomial>
mul_mult_poly(const UnivariatePolynomial &a, const MultivariatePolynomial &b);

RCP<const MultivariatePolynomial> add_mult_poly(const UnivariatePolynomial &a,
                                                const UnivariatePolynomial &b);
RCP<const MultivariatePolynomial> sub_mult_poly(const UnivariatePolynomial &a,
                                                const UnivariatePolynomial &b);
RCP<const MultivariatePolynomial> mul_mult_poly(const UnivariatePolynomial &a,
                                                const UnivariatePolynomial &b);

class MultivariateExprPolynomial
{
private:
    RCP<const MultivariatePolynomial> poly_;

public:
    MultivariateExprPolynomial()
    {
    }
    ~MultivariateExprPolynomial() SYMENGINE_NOEXCEPT
    {
    }
    MultivariateExprPolynomial(const MultivariateExprPolynomial &) = default;
    MultivariateExprPolynomial(MultivariateExprPolynomial &&other)
        SYMENGINE_NOEXCEPT : poly_(std::move(other.poly_))
    {
    }
    MultivariateExprPolynomial(RCP<const MultivariatePolynomial> p)
        : poly_(std::move(p))
    {
    }
    explicit MultivariateExprPolynomial(int i)
    {
        vec_sym s;
        vec_int v;
        poly_ = MultivariatePolynomial::multivariate_polynomial(
            s, {{v, Expression(i)}});
    }

    explicit MultivariateExprPolynomial(Expression e)
    {
        vec_sym s;
        vec_int v;
        poly_ = MultivariatePolynomial::multivariate_polynomial(s, {{v, e}});
    }
    explicit MultivariateExprPolynomial(RCP<const Symbol> sym)
    {
        vec_sym s;
        vec_int v;
        poly_ = MultivariatePolynomial::multivariate_polynomial(
            s, {{v, Expression(sym)}});
    }
    MultivariateExprPolynomial &operator=(const MultivariateExprPolynomial &)
        = default;
    MultivariateExprPolynomial &
    operator=(MultivariateExprPolynomial &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other)
            this->poly_ = std::move(other.poly_);
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const MultivariateExprPolynomial &expr)
    {
        os << expr.poly_->__str__();
        return os;
    }

    friend MultivariateExprPolynomial
    operator+(const MultivariateExprPolynomial &a,
              const MultivariateExprPolynomial &b)
    {
        return MultivariateExprPolynomial(add_mult_poly(*a.poly_, *b.poly_));
    }

    MultivariateExprPolynomial &
    operator+=(const MultivariateExprPolynomial &other)
    {
        poly_ = add_mult_poly(*poly_, *other.poly_);
        return *this;
    }

    friend MultivariateExprPolynomial
    operator-(const MultivariateExprPolynomial &a,
              const MultivariateExprPolynomial &b)
    {
        return MultivariateExprPolynomial(sub_mult_poly(*a.poly_, *b.poly_));
    }

    MultivariateExprPolynomial operator-() const
    {
        MultivariateExprPolynomial retval(*this);
        neg_mult_poly(*(retval.poly_.ptr()));
        return retval;
    }

    MultivariateExprPolynomial &
    operator-=(const MultivariateExprPolynomial &other)
    {
        poly_ = sub_mult_poly(*poly_, *other.poly_);
        return *this;
    }

    friend MultivariateExprPolynomial
    operator*(const MultivariateExprPolynomial &a,
              const MultivariateExprPolynomial &b)
    {
        return MultivariateExprPolynomial(
            mul_mult_poly(*(a.poly_), *(b.poly_)));
    }

    MultivariateExprPolynomial &
    operator*=(const MultivariateExprPolynomial &other)
    {
        poly_ = mul_mult_poly(*poly_, *(other.poly_));
        return *this;
    }

    bool operator==(const MultivariateExprPolynomial &other) const
    {
        return eq(*poly_, *other.poly_);
    }

    bool operator==(const Expression &other) const
    {
        if (poly_->dict_.size() > 1)
            return false;
        if (poly_->dict_.size() == 0) {
            if (other == Expression(0)) {
                return true;
            } else {
                return false;
            }
        }
        return poly_->dict_.begin()->second == other;
    }

    bool operator!=(const MultivariateExprPolynomial &other) const
    {
        return not(*this == other);
    }

    const RCP<const Basic> get_basic() const
    {
        return poly_;
    }

}; // MultivariateExprPolynomial

} // SymEngine

#endif
