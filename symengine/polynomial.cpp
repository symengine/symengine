#include <symengine/polynomial.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/constants.h>

namespace SymEngine {

UnivariateIntPolynomial::UnivariateIntPolynomial(const RCP<const Symbol> &var, const unsigned int &degree, map_uint_mpz&& dict) :
     degree_{degree}, var_{var}, dict_{std::move(dict)} {

    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

bool UnivariateIntPolynomial::is_canonical(const unsigned int &degree_, const map_uint_mpz& dict) const
{
    if (dict.size() != 0) {
        unsigned int prev_degree = (--dict.end())->first;
        if (prev_degree != degree_)
            return false;
    } else if (0 != degree_) {
        return false;
    }
    //Check if dictionary contains terms with coeffienct 0
    for (auto itter = dict.begin(); itter != dict.end(); itter++) {
        if (0 == itter->second)
            return false;
    }
    return true;
}

std::size_t UnivariateIntPolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEINTPOLYNOMIAL;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : this->dict_) {
        std::size_t temp = UNIVARIATEINTPOLYNOMIAL;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, it.second.get_si());
        seed += temp;
    }
    return seed;
}

bool UnivariateIntPolynomial::__eq__(const Basic &o) const
{
    if (eq(*var_, *(static_cast<const UnivariateIntPolynomial &>(o).var_)) and
        map_uint_mpz_eq(dict_, static_cast<const UnivariateIntPolynomial &>(o).dict_))
        return true;

    return false;
}

int UnivariateIntPolynomial::compare(const Basic &o) const
{
    const UnivariateIntPolynomial &s = static_cast<const UnivariateIntPolynomial &>(o);

    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_mpz_compare(dict_, s.dict_);
}

RCP<const UnivariateIntPolynomial> UnivariateIntPolynomial::from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d)
{
    auto itter = d.begin();
    while (itter != d.end()) {
        if (integer_class(0) == itter->second) {
            auto toErase = itter;
            itter++;
            d.erase(toErase);
        } else {
            itter++;
	    }
    }
    unsigned int degree = 0;
    if(!d.empty())
        degree = (--(d.end()))->first;
    return make_rcp<const UnivariateIntPolynomial>(var, degree, std::move(d));
}

RCP<const UnivariateIntPolynomial> UnivariateIntPolynomial::from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v){
    map_uint_mpz dict;
    unsigned int degree = 0;
    for(unsigned int i = 0; i < v.size(); i++) {
        if (0 != v[i]) {
  	    dict.insert(std::pair<unsigned int, integer_class>(i, v[i]));
            degree = i;
        }
    }
    return make_rcp<const UnivariateIntPolynomial>(var, degree, std::move(dict));
}

void UnivariateIntPolynomial::dict_add_term(map_uint_mpz &d, const integer_class &coef, const unsigned int &n)
{
    auto it = d.find(n);
    if (it == d.end())
       d[n] = coef;
}

vec_basic UnivariateIntPolynomial::get_args() const {
    vec_basic args;
    map_uint_mpz d;
    for (const auto &p: dict_) {
        d = {{p.first, p.second}};
        if (d.begin()->first == 0) {
            args.push_back( integer(d.begin()->second));
        } else if (d.begin()->first == 1) {
            if (d.begin()->second == 1) {
                args.push_back(var_);
            } else {
                args.push_back(Mul::from_dict(integer(d.begin()->second), {{var_, one}}));
            }
        } else {
            if (d.begin()->second == 1) {
                args.push_back( pow(var_, integer(d.begin()->first)));
            } else {
                args.push_back( Mul::from_dict(integer(d.begin()->second),{{var_, integer(d.begin()->first)}}));
            }
        }
    }
    if (dict_.empty())
        args.push_back(zero);
    return args;
}

integer_class UnivariateIntPolynomial::max_coef() const {
    integer_class curr = dict_.begin()->second;
    for (const auto &it : dict_) {
        if (it.second > curr)
            curr = it.second;
    }
    return curr;
}

integer_class UnivariateIntPolynomial::eval(const integer_class &x) const {
    //TODO: Use Horner's Scheme
    integer_class ans(0), temp;
    for (const auto &p : dict_) {
        mp_pow_ui(temp, x, p.first);
        ans += p.second * temp;
    }
    return ans;
}

integer_class UnivariateIntPolynomial::eval_bit(const int &x) const {
    //TODO: Use Horner's Scheme
    integer_class ans(0);
    for (const auto &p : dict_) {
        integer_class t = integer_class(1) << x * p.first;
        ans += p.second * t;
    }
    return ans;
}

bool UnivariateIntPolynomial::is_zero() const {
    if (dict_.empty())
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_one() const {
    if (dict_.size() == 1 and dict_.begin()->second == 1 and
            dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_minus_one() const {
    if (dict_.size() == 1 and dict_.begin()->second == -1 and
            dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_integer() const {
    if(dict_.empty())
        return true;
    if (dict_.size() == 1 and dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_symbol() const {
    if (dict_.size() == 1 and dict_.begin()->first == 1 and
            dict_.begin()->second == 1)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_mul() const {
    if (dict_.size() == 1 and dict_.begin()->first != 0 and
            dict_.begin()->second != 1 and dict_.begin()->second != 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_pow() const {
    if (dict_.size() == 1 and dict_.begin()->second == 1 and
            dict_.begin()->first != 1 and dict_.begin()->first != 0)
        return true;
    return false;
}

RCP<const UnivariateIntPolynomial> add_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b) {
    map_uint_mpz dict;
    for (const auto &it : a.dict_)
        dict[it.first] = it.second;
    for (const auto &it : b.dict_)
        dict[it.first] += it.second;

    RCP<const UnivariateIntPolynomial> c = UnivariateIntPolynomial::from_dict(a.var_, std::move(dict));
    return c;
}

RCP<const UnivariateIntPolynomial> neg_poly(const UnivariateIntPolynomial &a) {
    map_uint_mpz dict;
    for (const auto &it : a.dict_)
        dict[it.first] = -1 * it.second;

    RCP<const UnivariateIntPolynomial> c = UnivariateIntPolynomial::from_dict(a.var_, std::move(dict));
    return c;
}

RCP<const UnivariateIntPolynomial> sub_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b) {
    map_uint_mpz dict;
    for (const auto &it : a.dict_)
        dict[it.first] = it.second;
    for (const auto &it : b.dict_)
        dict[it.first] -= it.second;

    RCP<const UnivariateIntPolynomial> c = UnivariateIntPolynomial::from_dict(a.var_, std::move(dict));
    return c;
}

//Calculates bit length of number, used in mul_poly() only
template <typename T>
unsigned int bit_length(T t){
    unsigned int count = 0;
    while (t > 0) {
        count++;
        t = t >> 1;
    }
    return count;
}

RCP<const UnivariateIntPolynomial> mul_poly(RCP<const UnivariateIntPolynomial> a, RCP<const UnivariateIntPolynomial> b) {
    //TODO: Use `const RCP<const UnivariateIntPolynomial> &a` for input arguments,
    //      even better is use `const UnivariateIntPolynomial &a`
    unsigned int da = a->degree_;
    unsigned int db = b->degree_;

    int sign = 1;
    if ((--(a->dict_.end()))->second < 0) {
        a = neg_poly(*a);
        sign = -1 * sign;
    }
    if ((--(b->dict_.end()))->second < 0) {
        b = neg_poly(*b);
        sign = -1 * sign;
    }

    mpz_class p = std::max(a->max_coef(), b->max_coef());
    unsigned int N = bit_length(std::min(da + 1, db + 1)) + bit_length(p) + 1;

    mpz_class a1 = 1 << N;
    mpz_class a2 = a1 / 2;
    mpz_class mask = a1 - 1;
    mpz_class sa = a->eval_bit(N);
    mpz_class sb = b->eval_bit(N);
    mpz_class r = sa*sb;

    std::vector<mpz_class> v;
    mpz_class carry = 0;

    while (r != 0 or carry != 0) {
        mpz_class b;
        mpz_and(b.get_mpz_t(), r.get_mpz_t(), mask.get_mpz_t());
        if (b < a2) {
            v.push_back(b + carry);
            carry = 0;
        } else {
            v.push_back(b - a1 + carry);
            carry = 1;
        }
        r >>= N;
    }

    if (sign == -1)
        return neg_poly(*UnivariateIntPolynomial::from_vec(a->var_, v));
    else
        return UnivariateIntPolynomial::from_vec(a->var_, v);
}


UnivariatePolynomial::UnivariatePolynomial(const RCP<const Symbol> &var, const unsigned int &degree, map_uint_Expr&& dict) :
     degree_{degree}, var_{var}, dict_{std::move(dict)} {

    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

UnivariatePolynomial::UnivariatePolynomial(const RCP<const Symbol> &var, const std::vector<Expression> &v) : var_{var} 
{

    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0)
            dict_add_term(dict_, v[i], i);
    }
    degree_ = v.size() - 1;

    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

bool UnivariatePolynomial::is_canonical(const unsigned int &degree_, const map_uint_Expr& dict) const
{
    map_uint_Expr ordered(dict.begin(), dict.end());
    unsigned int prev_degree = (--ordered.end())->first;
    if (prev_degree != degree_)
        return false;

    return true;
}

std::size_t UnivariatePolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEPOLYNOMIAL;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : this->dict_)
    {
        std::size_t temp = UNIVARIATEPOLYNOMIAL;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<Basic>(temp, *(it.second.get_basic()));
        seed += temp;
    }
    return seed;
}

bool UnivariatePolynomial::__eq__(const Basic &o) const
{
    if (eq(*var_, *(static_cast<const UnivariatePolynomial &>(o).var_)) and
        map_uint_Expr_eq(dict_, static_cast<const UnivariatePolynomial &>(o).dict_))
        return true;

    return false;
}

int UnivariatePolynomial::compare(const Basic &o) const
{
    const UnivariatePolynomial &s = static_cast<const UnivariatePolynomial &>(o);

    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_Expr_compare(dict_, s.dict_);
}

RCP<const Basic> UnivariatePolynomial::from_dict(const RCP<const Symbol> &var, map_uint_Expr &&d)
{
    if (d.size() == 1) {
        if (d.begin()->first == 0)
            return d.begin()->second.get_basic();
        else if (d.begin()->first == 1) {
            if (d.begin()->second == 0)
                return zero;
            else if (d.begin()->second == 1)
                return var;
            else if (is_a<Integer>(*d.begin()->second.get_basic()))
                return Mul::from_dict(rcp_static_cast<const Integer>(d.begin()->second.get_basic()), 
                    {{var, one}});
            else
                return mul(d.begin()->second.get_basic(), var);
        } else {
            if (d.begin()->second == 0)
                return zero;
            else if (d.begin()->second == 1)
                return pow(var, integer(d.begin()->first));
            else if (is_a<Integer>(*d.begin()->second.get_basic()))
                return Mul::from_dict(rcp_static_cast<const Integer>(d.begin()->second.get_basic()),
                    {{var, integer(d.begin()->first)}});
            else
                return pow(mul(d.begin()->second.get_basic(), var), integer(d.begin()->first));
        }
    } else {
        return make_rcp<const UnivariatePolynomial>(var, (--(d.end()))->first, std::move(d));
    }
}

void UnivariatePolynomial::dict_add_term(map_uint_Expr &d, const Expression &coef, const unsigned int &n)
{
    auto it = d.find(n);
    if (it == d.end())
       d[n] = coef;
}

vec_basic UnivariatePolynomial::get_args() const {
    vec_basic args;
    for (const auto &p: dict_) {
        args.push_back(UnivariatePolynomial::from_dict(var_, {{p.first, p.second}}));
    }
    return args;
}

Expression UnivariatePolynomial::max_coef() const {
    Expression curr = dict_.begin()->second;
    for (const auto &it : dict_)
        if (curr.get_basic()->__cmp__(*it.second.get_basic()))
            curr = it.second;
    }
    return curr;
}

Expression UnivariatePolynomial::eval(const Expression &x) const {
    //TODO: Use Horner's Scheme
    Expression ans = 0;
    for (const auto &p : dict_) {
        Expression temp;
        temp = pow_ex(x, Expression(p.first));
        ans += p.second * temp;
    }
    return ans;
}

bool UnivariatePolynomial::is_zero() const {
    if (dict_.size() == 1 and dict_.begin()->second == 0)
        return true;
    return false;
}

bool UnivariatePolynomial::is_one() const {
    if (dict_.size() == 1 and dict_.begin()->second == 1 and
            dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariatePolynomial::is_minus_one() const {
    if (dict_.size() == 1 and dict_.begin()->second == -1 and
            dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariatePolynomial::is_integer() const {
    if (dict_.size() == 1 and dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariatePolynomial::is_symbol() const {
    if (dict_.size() == 1 and dict_.begin()->first == 1 and
            dict_.begin()->second == 1)
        return true;
    return false;
}

bool UnivariatePolynomial::is_mul() const {
    if (dict_.size() == 1 and dict_.begin()->first != 0 and
            dict_.begin()->second != 1 and dict_.begin()->second != 0)
        return true;
    return false;
}

bool UnivariatePolynomial::is_pow() const {
    if (dict_.size() == 1 and dict_.begin()->second == 1 and
            dict_.begin()->first != 1 and dict_.begin()->first != 0)
        return true;
    return false;
}

RCP<const UnivariatePolynomial> add_uni_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b) {
    map_uint_Expr dict;
    for (const auto &it : a.get_dict())
        dict[it.first] = it.second;
    for (const auto &it : b.get_dict())
        dict[it.first] += it.second;

    return univariate_polynomial(a.get_var(), (--(dict.end()))->first, std::move(dict));
}

RCP<const UnivariatePolynomial> neg_uni_poly(const UnivariatePolynomial &a) {
    map_uint_Expr dict;
    for (const auto &it : a.get_dict())
        dict[it.first] = -1 * it.second;

    return univariate_polynomial(a.get_var(), (--(dict.end()))->first, std::move(dict));
}

RCP<const UnivariatePolynomial> sub_uni_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b) {
    map_uint_Expr dict;
    for (const auto &it : a.get_dict())
        dict[it.first] = it.second;
    for (const auto &it : b.get_dict())
        dict[it.first] -= it.second;
    return univariate_polynomial(a.get_var(), (--(dict.end()))->first, std::move(dict));
}

RCP<const UnivariatePolynomial> mul_uni_poly(RCP<const UnivariatePolynomial> a, RCP<const UnivariatePolynomial> b) {
    // throw std::runtime_error("Not Implemented");
    map_uint_Expr dict;
    for (const auto &i1 : a->get_dict())
        for (const auto &i2 : b->get_dict())
            dict[i1.first + i2.first] += i1.second * i2.second;
    return univariate_polynomial(a->get_var(), (--(dict.end()))->first, std::move(dict));
}

  
///Multivariate Polynomial///

MultivariateIntPolynomial::MultivariateIntPolynomial(const set_sym &vars, umap_sym_uint &degrees, umap_uvec_mpz &dict) :
vars_{std::move(vars)}, degrees_{std::move(degrees)}, dict_{std::move(dict)} {
    SYMENGINE_ASSERT(is_canonical(vars_, degrees_, dict_))
}

RCP<const MultivariateIntPolynomial> MultivariateIntPolynomial::from_dict(const set_sym &s, umap_uvec_mpz &&d) {
    umap_sym_uint degs;
    auto itter = d.begin();
    while (itter != d.end()) {
        if (integer_class(0) == itter->second) {
            auto toErase = itter;
            itter++;
            d.erase(toErase);
        } else {
            itter++;
        }
    }
    
    int whichvar = 0;
    for(auto sym : s){
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(sym,0));
        for(auto bucket : d){
            if(bucket.first[whichvar] > degs.find(sym)->second)
                degs.find(sym)->second = bucket.first[whichvar];
        }
        whichvar++;
    }
    return make_rcp<const MultivariateIntPolynomial>(s,degs,d);
}
  
vec_basic  MultivariateIntPolynomial::get_args() const{
    vec_basic args;
    umap_uvec_mpz d;
    for(const auto &p : dict_){
        d = {{p.first, p.second}};
        map_basic_basic b;
        int whichvar = 0;
        for (auto sym : vars_) {
            b.insert(std::pair<RCP<const Basic>, RCP<const Basic>>(sym, make_rcp<Integer>(integer_class(d.begin()->first[whichvar]))));
            whichvar++;
        }
	args.push_back(Mul::from_dict(make_rcp<const Integer>(d.begin()->second), std::move(b)));
    }
    return args;
}
  
bool MultivariateIntPolynomial::is_canonical(const set_sym &vars, const umap_sym_uint &degrees, const umap_uvec_mpz &dict){
    //checks that the dictionary does not contain terms with coefficient 0
    for (auto bucket : dict) {
        if(integer_class(0) == bucket.second)
	    return false;
    }
  
    //checks that the maximum degree of any variable is correct according to the dictionary
    unsigned int whichvar = 0; //keeps track of the index of the variable we are checking
    for(auto var : vars){
        unsigned int maxdegree = 0;
        for(auto bucket : dict){
	    if(bucket.first[whichvar] > degrees.find(var)->second)
	        return false;
	    else if(maxdegree < bucket.first[whichvar] )
	        maxdegree = bucket.first[whichvar];
        }
        if(maxdegree != degrees.find(var)->second)
	    return false;
        whichvar++;
    }
    return true;
}

std::size_t MultivariateIntPolynomial::__hash__() const{
    std::hash<std::string> hash_string;
    std::size_t seed = 0;
    for(auto var : vars_)
        seed ^= hash_string(var->get_name()) + 0x9e3779b + (seed << 6) + (seed >> 2); //boost's method for combining hashes
    for(auto bucket : dict_){
        seed ^= vec_uint_hash()(bucket.first) + 0x9e3779b + (seed << 6) + (seed >> 2);
        seed ^= mpz_hash(bucket.second) + 0x9e3779b + (seed << 6) + (seed >> 2);
    }
    return seed;
}

bool MultivariateIntPolynomial::__eq__(const Basic &o) const{
    return ( set_eq<set_sym>(vars_, static_cast<const MultivariateIntPolynomial &>(o).vars_) && umap_uvec_mpz_eq(dict_, static_cast<const MultivariateIntPolynomial &>(o).dict_) );
}

int MultivariateIntPolynomial::compare(const Basic &o) const{
    //copied from UnivariateIntPolynomial::compare and then modified.
    const MultivariateIntPolynomial &s = static_cast<const MultivariateIntPolynomial&>(o);
    
    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    int cmp = set_compare<set_sym>(vars_, s.vars_);
    if(cmp != 0)
        return cmp;

    return umap_uvec_mpz_compare(dict_, s.dict_); 
}

integer_class MultivariateIntPolynomial::eval(std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> &vals){
    integer_class ans(0);
    for(auto bucket : dict_) {
        integer_class term(1);
        unsigned int whichvar = 0;
        for(auto sym : vars_){
            integer_class temp;
            mp_pow_ui(temp, vals.find(sym)->second, bucket.first[whichvar] );
            term *= temp;
            whichvar++;
        }
        ans += term;
    } 
    return ans;
}

std::string MultivariateIntPolynomial::toString() const{
    std::ostringstream s;
    bool first = true; //is this the first term being printed out?
    std::vector<vec_uint> v;
    //To change the ordering in which the terms will print out, change
    //vec_uint_compare in dict.h
    for(auto bucket : dict_){
        auto it = v.begin();
	while(it != v.end() && vec_uint_compare()(bucket.first,*it)){
	    it++;
	}
	v.insert(it, bucket.first);
    }

    for(vec_uint exps : v){
        integer_class c = dict_.find(exps)->second;
	if(c != 0){
	    if(c > 0 && !first)
    	        s << "+ ";
	    else if(c < 0)
	        s << "- "; 
        unsigned int i = 0;
	    std::ostringstream expr;
        for(auto it :vars_)
        {
	        if(dict_.find(exps)->first[i] != 0){
		        expr << it->get_name();
		        if(dict_.find(exps)->first[i] > 1)
		            expr << "**" << dict_.find(exps)->first[i];
	            expr << " ";
	        }
	        i++;
        }
	    if(mp_abs(c) != 1 || expr.str().empty())
	        s << mp_abs(c) << "*";
	    s << expr.str();
	    first = false;
	}
    }
    
    if(s.str().empty())
        s << "0 ";
    std::string final(s.str());
    final.pop_back();
    return final;
}

unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_sym &s, const set_sym &s1, const set_sym &s2){
    auto a1 = s1.begin();
    auto a2 = s2.begin();
    unsigned int poscount = 0;
    while(a1 != s1.end() && a2 != s2.end()){
        if(0 == (*a1)->compare(**a2) && (a1 != s1.end() && a2 != s2.end())){
            v1.insert(v1.end(), poscount);
	    v2.insert(v2.end(), poscount);
	    s.insert(*a1);
	    a1++;
	    a2++;
        } else if(-1 == (*a1)->compare(**a2)){
	    v1.insert(v1.end(),poscount);
            s.insert(*a1);
	    a1++;
        } else if(1 == (*a1)->compare(**a2)){
	    v2.insert(v2.end(),poscount);
	    s.insert(*a2);
	    a2++;
        }
	poscount++;
    }
    if(a1 == s1.end()){
        while(a2 != s2.end()){
	    v2.insert(v2.end(),poscount);
	    s.insert(*a2);
	    a2++;
	    poscount++;
        }
    } else if(a2 == s2.end()){
        while(a1 != s1.end()){
	    v1.insert(v1.end(),poscount);
            s.insert(*a1);
            a1++;
            poscount++;
        }
    }
    return poscount; //return size of the new vectors
}

unsigned int reconcile(vec_uint &v1, unsigned int &v2, set_sym &s, const set_sym &s1, const RCP<const Symbol> s2){
    auto a1 = s1.begin();
    unsigned int poscount = 0;
    bool inserted = false;
    while(a1 != s1.end() && !inserted){
        if(0 == (*a1)->compare(*s2) ){
            v1.insert(v1.end(), poscount);
	    v2 = poscount;
	    s.insert(*a1);
	    a1++;
        } else if(-1 == (*a1)->compare(*s2)){
	    v1.insert(v1.end(),poscount);
            s.insert(*a1);
	    a1++;
        } else if(1 == (*a1)->compare(*s2)){
	    v2 = poscount;
	    s.insert(s2);
	    inserted = true;
        }
	poscount++;
    }
    while(a1 != s1.end()){
        v1.insert(v1.end(),poscount);
        s.insert(*a1);
        a1++;
        poscount++;
    }
    return poscount;
}
  
vec_uint translate(vec_uint original, vec_uint translator, unsigned int size){
    vec_uint changed;
    for(unsigned int i = 0; i < size; i++){
        changed.insert(changed.end(), 0);
    }
    for(unsigned int i = 0; i < original.size(); i++){
        changed[translator[i]] = original[i];
    }
    return changed;
}

vec_uint translate(unsigned int original, unsigned int translator, unsigned int size){
    vec_uint changed;
    for(unsigned int i = 0; i< size; i++){
        changed.insert(changed.end(),0);
    }
    changed[translator] = original;
    return changed;
}

RCP<const MultivariateIntPolynomial> add_mult_poly(const MultivariateIntPolynomial &a, const MultivariateIntPolynomial &b){
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1,v2,s,a.vars_,b.vars_);
    for(auto bucket : a.dict_){
        dict.insert(std::pair<vec_uint, integer_class>(translate(bucket.first,v1,size), bucket.second)); 
    }
    for(auto bucket : b.dict_){
        auto target = dict.find(translate(bucket.first,v2,size));
        if(target != dict.end()){
            target->second += bucket.second;
        } else{
            dict.insert(std::pair<vec_uint, integer_class>(translate(bucket.first,v2,size),bucket.second));
        }
    }
    unsigned int maxdegree = 0;
    for(auto bucket : s){
        maxdegree = 0;
        if(a.degrees_.find(bucket) != a.degrees_.end())
	    maxdegree = a.degrees_.find(bucket)->second;
	if(b.degrees_.find(bucket) != a.degrees_.end())
	    if(maxdegree <  b.degrees_.find(bucket)->second)
  	        maxdegree =  b.degrees_.find(bucket)->second;
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(bucket, maxdegree));
    }
    return make_rcp<const MultivariateIntPolynomial>(s,degs ,dict);
}

// adds together translate two vec_uints to the proper format and adds them together componentwise 
vec_uint uint_vec_translate_and_add(const vec_uint &v1, const vec_uint &v2,const vec_uint &translator1, const vec_uint &translator2, const unsigned int size){
    vec_uint result;
    for(unsigned int i = 0; i < size; i++){
        result.insert(result.end(), 0);
    }
    for(unsigned int i = 0; i < translator1.size(); i++){
        result[translator1[i]] += v1[i];
    }
    for(unsigned int i = 0; i < translator2.size(); i++){
        result[translator2[i]] += v2[i];
    }
    return result;
}

vec_uint uint_vec_translate_and_add(const vec_uint &v1, const unsigned int v2, const vec_uint &translator1,const unsigned int &translator2, const unsigned int size){
    vec_uint result;
    for(unsigned int i = 0; i < size; i++){
        result.insert(result.end(),0);
    }
    for(unsigned int i =0; i< translator1.size(); i++){
        result[translator1[i]] += v1[i];
    }
    result[translator2] += v2;
    return result;
}

RCP<const MultivariateIntPolynomial> neg_mult_poly(const MultivariateIntPolynomial &a){
    umap_uvec_mpz dict;
    set_sym s = a.vars_;
    umap_sym_uint degs =a.degrees_;
    for(auto bucket : a.dict_){
        dict.insert(std::pair<vec_uint, integer_class>(bucket.first, - bucket.second));
    }
    return make_rcp<const MultivariateIntPolynomial>(s, degs, dict);
}

RCP<const MultivariateIntPolynomial> sub_mult_poly(const MultivariateIntPolynomial &a, const MultivariateIntPolynomial &b){
    return(add_mult_poly(a, *neg_mult_poly(b)));
}

RCP<const MultivariateIntPolynomial> mul_mult_poly(const MultivariateIntPolynomial &a, const MultivariateIntPolynomial &b){
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1,v2,s,a.vars_,b.vars_);
    unsigned int maxdegree = 0;
    for(auto bucket : s){
        maxdegree = 0;
	if(a.degrees_.find(bucket) != a.degrees_.end())
	    maxdegree += a.degrees_.find(bucket)->second;
	if(b.degrees_.find(bucket) != b.degrees_.end())
	    maxdegree += b.degrees_.find(bucket)->second;
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(bucket,maxdegree));
    }
    for(auto a_bucket : a.dict_){
        for(auto b_bucket : b.dict_){
   	    vec_uint target = uint_vec_translate_and_add(a_bucket.first,b_bucket.first,v1,v2,size);
	    if(dict.find(target) == dict.end()){
                dict.insert(std::pair<vec_uint, integer_class>(target, a_bucket.second * b_bucket.second));
	    }
	    else{
  	        dict.find(target)->second += a_bucket.second * b_bucket.second;
	    }
        }
    }
    return make_rcp<const MultivariateIntPolynomial>(s, degs, dict);
}

RCP<const MultivariateIntPolynomial> add_mult_poly(const MultivariateIntPolynomial &a, const UnivariateIntPolynomial &b){
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1,v2,s,a.vars_,b.var_);
    for(auto bucket : a.dict_){
        dict.insert(std::pair<vec_uint, integer_class>(translate(bucket.first,v1,size),bucket.second ));
    }
    for(auto bucket : b.dict_){
        auto target = dict.find(translate(bucket.first, v2, size));
	if(target != dict.end()){
  	    target->second += bucket.second;
	} else{
 	    dict.insert(std::pair<vec_uint, integer_class>(translate(bucket.first, v2, size), bucket.second));
	}
    }
    unsigned int maxdegree = 0;
    for(auto bucket : s){
        maxdegree = 0;
        if(a.degrees_.find(bucket) != a.degrees_.end()){
	    maxdegree = a.degrees_.find(bucket)->second;
	}
        if(b.var_->__eq__(*bucket)){
	  if(maxdegree < b.degree_)
  	      maxdegree = b.degree_;
	}
	degs.insert(std::pair<RCP<const Symbol>, unsigned int>(bucket, maxdegree));
    }
    return make_rcp<MultivariateIntPolynomial>(s,degs,dict);
}

RCP<const MultivariateIntPolynomial> sub_mult_poly(const MultivariateIntPolynomial &a, const UnivariateIntPolynomial &b){
    return add_mult_poly(a, *neg_poly(b));
}

RCP<const MultivariateIntPolynomial> sub_mult_poly(const UnivariateIntPolynomial &a, const MultivariateIntPolynomial &b){
    return add_mult_poly(*neg_mult_poly(b),a);
}

RCP<const MultivariateIntPolynomial> mul_mult_poly(const MultivariateIntPolynomial &a, const UnivariateIntPolynomial &b){
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1,v2,s,a.vars_,b.var_);
    unsigned int maxdegree = 0;
    for(auto bucket : s){
        maxdegree = 0;
        if(a.degrees_.find(bucket) != a.degrees_.end())
	    maxdegree += a.degrees_.find(bucket)->second;
	if(bucket->__eq__(*b.var_))
	    maxdegree += b.degree_;
	degs.insert(std::pair<RCP<const Symbol>,unsigned int>(bucket,maxdegree));
    }
    for(auto a_bucket : a.dict_){
        for(auto b_bucket : b.dict_){
	    vec_uint target = uint_vec_translate_and_add(a_bucket.first,b_bucket.first,v1,v2,size);
	    if(dict.find(target) == dict.end()){
	        dict.insert(std::pair<vec_uint, integer_class>(target, a_bucket.second * b_bucket.second));
	    }else{
	        dict.find(target)->second += a_bucket.second * b_bucket.second;
	    }
        }
    }
    return make_rcp<const MultivariateIntPolynomial>(s,degs,dict);
}

RCP<const MultivariateIntPolynomial> add_mult_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b){
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    bool same = false; //are the variables of a and b the same?
    if(0 == a.var_->compare(*b.var_)){
        v1 = 0;
        v2 = 0;
        s.insert(a.var_);
	same = true;
    }else{
        if(-1 == a.var_->compare(*b.var_)){
            v1 = 0;
            v2 = 1;
        }else{
            v2 = 0;
            v1 = 1;
        }
	s.insert(a.var_);
        s.insert(b.var_);
    }

    for(auto bucket : a.dict_){
        vec_uint v;
	v.insert(v.end(),0);
	if(!same)
  	    v.insert(v.end(),0);
	v[v1] = bucket.first;
	dict.insert(std::pair<vec_uint, integer_class>(v,bucket.second));
    }
      for(auto bucket : b.dict_){
        vec_uint v;
	v.insert(v.end(),0);
	if(!same)
  	    v.insert(v.end(),0);
	v[v2] = bucket.first;
	auto target = dict.find(v);
	if(target != dict.end())
	    target->second += bucket.second;
	else
  	    dict.insert(std::pair<vec_uint, integer_class>(v,bucket.second));
    }
    if(!same){
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(a.var_,a.degree_));
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(b.var_,b.degree_));
    }else{
        if(a.degree_ > b.degree_){
	    degs.insert(std::pair<RCP<const Symbol>, unsigned int>(a.var_,a.degree_));
        }else{
	    degs.insert(std::pair<RCP<const Symbol>, unsigned int>(b.var_,b.degree_));
	}
    }
    return make_rcp<MultivariateIntPolynomial>(s,degs,dict);
}

RCP<const MultivariateIntPolynomial> sub_mult_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b){
    return add_mult_poly(a, *neg_poly(b));
}

RCP<const MultivariateIntPolynomial> mul_mult_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b){
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    bool same = false; //are the variables of a and b the same?
    if(0 == a.var_->compare(*b.var_)){
        v1 = 0;
        v2 = 0;
        s.insert(a.var_);
	same = true;
    }else{
        if(-1 == a.var_->compare(*b.var_)){
            v1 = 0;
            v2 = 1;
        }else{
            v2 = 0;
            v1 = 1;
        }
	s.insert(a.var_);
	s.insert(b.var_);
    }

    if(!same){
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(a.var_,a.degree_));
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(b.var_,b.degree_));
    }else{
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(a.var_,a.degree_ + b.degree_));
    }
      for(auto a_bucket : a.dict_){
      for(auto b_bucket : b.dict_){
	    vec_uint target;
	    target.insert(target.end(),0);
	    if(!same)
	        target.insert(target.end(),0);
	    target[v1] += a_bucket.first;
	    target[v2] += b_bucket.first;
	    if(dict.find(target) == dict.end())
	        dict.insert(std::pair<vec_uint, integer_class>(target, a_bucket.second * b_bucket.second));
	    else
	        dict.find(target)->second += a_bucket.second * b_bucket.second;
        }
    }
    return make_rcp<MultivariateIntPolynomial>(s,degs,dict);

}

} // SymEngine
