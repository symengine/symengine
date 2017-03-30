
# Table Of Contents

* [Personal Background](#personal-background)
  * [Details](#details)
  * [About Me](#about-me)
  * [Platform details](#platform-details)
* [Contributions to SymEngine](#contributions-to-symengine)
* [Contributions to SymPy](#contributions-to-sympy)
* [Project Overview](#project-overview)
* [Details of Project](#details-of-project)
  * [Set-Up for Solvers](#set-up-for-solvers--)
    * [Implementing GCD](#implementing-gcd--)
    * [Completing Factorisation](#completing-factorisation)
    * [Improve Set Module](#improve-set-module--)
  * [Univariate Solvers](#univariate-solvers---)
    * [Polynomial solvers](#polynomial-solvers)
    * [Trigonometric Solvers](#trigonometric-solvers)
    * [Linear System of Equations](#linear-system-of-equations-)
  * [Applications of Solvers](#applications-of-solvers)
* [Previous Implementation in SymPy](#previous-implementation-in-sympy)
* [Discussions](#discussions)
* [Timeline](#timeline)

# Personal Background

### Details

**Name** : Adha Ranjith Kumar

**University** : [Indian Institute of Technology, Kharagpur](http://www.iitkgp.ac.in)

**Email** : ranjith.dakshana2015@gmail.com

**GitHub** : [ranjithkumar007](https://github.com/ranjithkumar007) 

**Blog** : [ranjithkumar007.github.io](http://ranjithkumar007.github.io/)

**Time-zone** : IST (UTC +5:30)

### About Me

I am a second year undergraduate student pursuing Computer Science and Engineering at Indian Institute of Technology, Kharagpur.

I have been programming for about 2 years now. I started with C and C++. I've used C++ mainly for competitive programming contests where i got comfortable with STL. I learnt Python last year. I like the ease with which one can code in python. I also learnt web-scraping as a part of [mini-project](https://github.com/ranjithkumar007/PatternMatch) under [Harsh Gupta](https://github.com/hargup). I developed an Image Processing based project that analyses the impact of colors in movie posters and also mentored junior students in an IEEE certified Image Processing workshop. Apart from C++/C and Python, I did projects in Java and JavaScript. I am quite comfortable using git and github for version control.

I am also a robotics enthusiast and have been part of Kharagpur Robosoccer Students' Group ([KRSSG](http://www.krssg.in)) team as Artificial Intelligence team member. KRSSG made India's first ever participation in RoboCup's Humanoid Simulation League last year in Germany. I am working on Humanoid simulation since March 2016 and have implemented startegies for goalkeeper and positioning using vornoi and delaunay triangulation.

My educational background is sufficient for the project. I have taken the following mathematical courses: Discrete Mathematics, Calculus, Probability and Statistics; programming courses: Algorithms-I, Programming and Data Structures.

[Abinash Meher](https://github.com/abinashmeher999) introduced me to SymEngine last December and I have started contributing ever since. I learnt many new topics like lambda functions,CRTP and got familiar with `CMake`,`makefile` while contributing. 


### Platform details

**OS** : Ubuntu 14.04

**Editor** : SublimeText 3


### Contributions to SymEngine

* (**Open**) `Hensel lift and zassenhaus in Integer Domain` [#1227](https://github.com/symengine/symengine/pull/1227); This is a rebase over [#1066](https://github.com/symengine/symengine/pull/1066). Completed Hensel-lift in integer-domain. This is important for implementing solvers as `factor()` method needs `hensel_lift` in integer domain.

* (**Merged**) `increase coverage and minor bug fixes` [#1217](https://github.com/symengine/symengine/pull/1217); Several bugs were fixed along the due course of adding thorough test cases. CodeCoverage increased by more than 9.3% with this PR.

* (**Merged**) `Fix for #1154` [#1165](https://github.com/symengine/symengine/pull/1165); It fixed a bug in pow. Earlier, pow(base, expo) returned zero if base is zero but it should depend on the sign of expo.  Apparently, this fixed issue [#1154](https://github.com/symengine/symengine/issues/1154).

* (**Merged**) `use ComplexInf instead of throwing DivisionByZero error` [#1170](https://github.com/symengine/symengine/pull/1170); Instead of throwing error if denominator is zero, It is advisable to return ComplexInf or Nan depending on the numerator.

* (**Merged**) `edit a misleading variable name` [#1163](https://github.com/symengine/symengine/pull/1163); It was a minor fix for a misleading variable name. This is my first merged PR.

* (**Merged**) `Improve Log` [#1172](https://github.com/symengine/symengine/pull/1172); This PR improved Log function by implementing Log for negative arguments. To facilitate implementation of Inf values as suggested by [isuruf](https://github.com/isuruf), Evaluate Class was added for Infinity.

* (**Merged**) `add asech to evaluate class` [#1189](https://github.com/symengine/symengine/pull/1189); Implemented asech function in evaluate class.

* (**Merged**) `add erf and erfc to evaluate class` [#1192](https://github.com/symengine/symengine/pull/1192); Implemented erf and erfc function in evaluate class.

* (**Merged**) `move Log to functions.h` [#1195](https://github.com/symengine/symengine/pull/1195);
 As Log is a OneArgFunction, It should better be placed inside `functions.cpp` rather than `pow.cpp`

* (**Merged**) `add Symbolic constants` [#1196](https://github.com/symengine/symengine/pull/1196);
 Added two more symbolic constants namely Catalan's constant and GoldenRatio.

* (**Merged**) `add evaluate class for nan` [#1202](https://github.com/symengine/symengine/pull/1202); Implemented evaluate class for nan.

* (**Merged**) `change SymEngineException to NotImplemented wherever applicable` [#1201](https://github.com/symengine/symengine/pull/1201); While implementing symbolic constants, I found many occurences where `NotImplementedError` could have been used instead of `SymEngineException`. Fixed with this PR.

* (**Merged**) `implement digamma,trigamma using polygamma` [#1203](https://github.com/symengine/symengine/pull/1203); Implemented digamma and trigamma functions using already implemented polygamma.

* (**Merged**) `bug fix in upper and lower gamma` [#1204](https://github.com/symengine/symengine/pull/1204); fixed a non-trivial bug in upper and lower gamma. Along with this, I added base cases for both these using erf and erfc functions.

* (**Merged**) `add xor,xnor and tests` [#1206](https://github.com/symengine/symengine/pull/1206);
 Implemented missing xor and xnor in the logic module.

* (**Open**) `more error functions` [#1208](https://github.com/symengine/symengine/pull/1208);
 Implemented inverses of error function,complementary error function and Two-argument error function.
 Also, Added Derivatives of all these three functions.

* (**Open**) `auto eval if base is E and expo is log()` [#1207](https://github.com/symengine/symengine/pull/1207); Evaluating expressions of type `e^(log)` automatically and of type `e^(log +-* log...)` only when `expand()` is invoked. 

* (**Merged**) `remove redundant check` [#1216](https://github.com/symengine/symengine/pull/1216);

* (**Merged**) `remove outdated comments` [#1193](https://github.com/symengine/symengine/pull/1193);

* (**Merged**) `Make formatting test fail more clear` [#1240](https://github.com/symengine/symengine/pull/1240); Added a travis test dedicated only for checking formatting.

* (**Merged**) `fix bug in eval mpc ` [#1210](https://github.com/symengine/symengine/pull/1210);
 Earlier, acsc and asec evaluated inverses of acos and asin. But It should be reverse. This trivial bug existed because of lack of test cases. So I started working on [#1217](https://github.com/symengine/symengine/pull/1217).

### Contributions to SymPy

* (**Merged**) `fix bug in extract_multiplicatively()` [#12270](https://github.com/sympy/sympy/pull/12270); Inherently, Fixed issue [#12254](https://github.com/sympy/sympy/issues/12254).
  
* (**Merged**) `add xnor and tests` [#12119](https://github.com/sympy/sympy/pull/12119); 
 Added Xnor in logic module of SymPy. I encountered it, While I was going through the docs of SymPy to [implement xor,xnor](https://github.com/symengine/symengine/pull/1206) for SymEngine.

----------

# Project Overview

Solving equations and system of equations is a crucial feature of Computer Algebra Systems and Polynomial factorization is one of the fundamental tools to implement solvers. Currently, SymEngine has implementation of Univariate Polyomials, MultiVariate Polynomials, Sets and Finite Fields. Most of the necessary tools for Solvers have been developed last year by [Srajan Garg](https://github.com/srajangarg) and [Nishant Nikhil](https://github.com/nishnik).

SymPy has a robust solvers module. I would like to port basic solvers like univariate solvers, trigonometric solvers and System of linear Equations to SymEngine. SymEngine's Set module is not matured enough, So I will also improve Sets to facilitate Solvers. In SymPy, Currently Major work is going around replacing ```solve``` with ```solveset```. So, rather that porting```solve```, I would port ```solveset``` to SymEngine. After implementing basic solvers module, I would also implement some applications of solvers like ```eigenvectors``` and ```eigenvalues``` of a matrix.

----------

# Details of Project

### Set-Up for Solvers : <br>
Before Proceeding to Solvers, I would like to complete Polynomial factorisation. Currently, SymEngine has Cantor Zassenhaus method for factoring polynomials in finite fields and also hensel lifting to convert roots back to integer domain. Remaining Aspect here is implementing a ```factor()``` method that actually returns the factors in integer domain.
For this, I need to implement the ```get square free part()``` method which in turn needs computation of ```gcd()``` in integer domain which is still unimplemented in SymEngine.

### Implementing GCD : <br>
The GCD of two polynomials is a polynomial, of the highest possible degree, that is a factor of both the two original polynomials. This algorithm is similar to trivial gcd that one learns in schools :

```
gcd(f, g):
    if deg(f) < deg(g)
      swap f, g

    if g = 0
      return f
    else
      return gcd(g, f mod g)
```
However this method turns out to be very slow(O(N^2)). This [paper](http://dl.acm.org/citation.cfm?doid=220346.220376) describes a faster and heuristics based algorithm. 

The algorithm computes the polynomial GCD by evaluating polynomials f and g at certain points and computing (fast) integer GCD of those
evaluations. The polynomial GCD is recovered from the integer image by interpolation. The final step is to verify if the result is the
correct GCD. If the verfication turns out to be false, then algorithm tries to run again with a different set of points. Algorithm runs for at most 6(GCDHUE_LIMIT) times.

LCM can be found directly if we can compute GCD as 
```
lcm(f,g) = (f*g)/gcd(f,g)
```
function prototypes would be similar to other functions in Univariate Polynomials.

Function Prototype : 
```
template <typename Poly> RCP<const Poly> gcd_poly(const Poly &a, const Poly &b) 
```
Example : 
```

In[] : gcd_poly(x**2-3*x+2,x-2)
Out[] : x - 2

In[] : gcd_poly(x**2-3*x+2,x+2)
Out[] : x**3−x**2−4*x+4
```

Wrappers for FLINT and Piranha are already implemented in SymEngine.

### Completing Factorisation
Once GCD is complete, ```zz_get_square_free_part()``` is implemented and then setup for ```zz_factor()``` is ready and can be easily implemented along the lines of implementation in SymPy.

Function Prototype
```
std::set<std::pair<integer_class,RCP<const UIntPoly>>, FactorKeyLess>> zz_factor() const;
```
Examples : 

```
In[] : zz_factor(x**3-1)
Out[] : [{1,x-1},{1,x**2+x+1}]

In[] : zz_factor(x**2-1)
Out[] : [{2,x-1}]
```

As it is clear from the function prototype, We need to implement a custom functor ```FactorKeyLess``` for defining the order of storing in set.

### Improve Set module : <br>

Mathematically solving an equation means returning the set where the equation is true.Sets are the most generic and can consistently represent all type of solutions. Also, We can use ```ImageSet``` i proposed to implement, to represent even generic solutions like ```{n*pi,for all n in Interval(-oo,+oo)}``` for equation ```sin(x) = 0```.
SymPy is also rewriting its ```solve()``` to a new ```solveset()``` which would eventually be renamed to ```solve()``` depricating older ```solve()```.
Status of Sets Module in SymEngine : 
Presently we have the following sub-modules implemented in Sets. 

- `FiniteSet`
- `EmptySet`
- `UniversalSet`
- `Interval`
- `Union`
- `Intersection`

We need a more robust implementation of Sets to facilitate solvers module. For this i am planning to implement the following in sets :

- `Complement`
- `ConditionSet`
- `ImageSet`

#### Complement : <br>
It is the relative complement of a set with another set. All elements that are present in A and not in B come under Complement(A,B).
If Only One argument is passed, A is assumed to be Universal Set. ```Complement``` would be implemented as a dervied class of ```Set```.

Prototype : 
```
class Complement : public Set {};   #Class Prototype
RCP<const Set> set_complement(const RCP<const Set> &a,const RCP<const Set> &b)  # Function Prototype

```
Example : 
```
In[] : complement(FiniteSet(0, 1, 2, 3, 4, 5), FiniteSet(1))
Out[] : {0,2,3,4,5}
```

#### ConditionSet: <br>
It contains a Set of elements which satisfies a given condition.
Mathematically, Its set-builder form is ```{x | condition(x) is True for x in S} ```.
Similar to Complement, ```ConditionSet``` would be derived class of ```Set```

Prototype : 
```
class ConditionSet : public Set {};   #Class Prototype
```
Example : 
```
setC = ConditionSet(x, x**2 > 4, Interval(0,100))
setC->contains(5) #True
setC->contains(2) #False
```

#### ImageSet :<br>
It is an Image of a set under a mathematical function. If this function can't compute the image, it returns an unevaluated ImageSet object.
Its set-builder form is { f(x) | x \in this }.

Prototype : 
```
class ImageSet : public Set {};   #Class Prototype
```

Example : 
```
In[] : imageset(x, x**2, Interval(0, 4))
Out[] : [0, 16]
```

### Univariate Solvers - <br>


#### Polynomial solvers
First, I would like to implement Polynomial solvers.

**Definition** - 
- **Cyclotomic Polynomial**: the nth cyclotomic polynomial, for any positive integer `n`, is the unique irreducible polynomial with integer coefficients, which is a divisor of  ```x**n-1``` and is not a divisor of ```x**k-1``` for any ```k < n```. Its roots are all nth primitive roots of unity ```e**(2*i*pi*k/n)```, where `k` runs over the positive integers not greater than `n` and coprime to `n`.
Help from [here](http://mathworld.wolfram.com/CyclotomicPolynomial.html)<br>

Examples :
```
  x**6+x**5+x**4+x**3+x**2+x+1
  x**4+1
```

This module is divided into three sub modules.<br/>
- **Polynomials of lower degree(degree<=4)** : <br/>
      <ul> 
      <li>**Linear** (degree = 1)<br/></li>
        root is trivial that is x = -b/a<br/>
      <li> **Quadratic** (degree = 2)<br/></li>
        finding roots is straight forward here as well using school-grade formula.<br/>
      <li> **Cubic** (degree = 3)<br/></li>
        general formula exists for cubic polynomials. Reference from [wikipedia](https://en.wikipedia.org/wiki/Cubic_function#General_formula).<br/>
      <li> **Quartic** (degree = 4)<br/></li>
         In general ,we can find roots of quartic polynomials using Descartes-Euler algorithm as explained [here](http://eqworld.ipmnet.ru/en/solutions/ae/ae0108.pdf).<br/>
      </ul>

- **cyclotomic polynomials**
       - For solving cyclotomic polynomials, we need to know its order n.
        this can be found using inverse totient function after which roots can be calculated easily as ```e**(k*2*pi*I/n)``` , for ```1<=k<=n``` and `n`, `k` being co-prime.

- **integrating ```zz_factor()``` to solve polynomials that don't fall in the above categories**.
        <ul>
        <li>remaining set of polynomials are factorised using ```zz_factor()``` method that internally converts coefficients to galois fields, finds roots using Cantor-Zassenhaus algorithm and converts them back to integer domain using hensel-lifting. After factorising, We have a set of factors whose roots can be found using above methods and final set of roots are returned.</li></ul>

All these Polynomial root-finding algorithms would be wrapped under a single function ```solve_poly()``` which is directly callable but would be recommended to call through ```solve()``` which internally calls ```solve_poly()```.
Essential helper functions like ```_invert_real()``` would be implemented along the way.


Prototype :
```
template <typename Poly> RCP<const Set> solve_poly(const Poly& p,RCP<const Symbol> &x,
                              RCP<const Set> domain = Interval(-Inf,+Inf))
```

Examples : 
```
In[] : solve(x**2-2*x+1,x,FiniteSet(2,3,4))
Out[] : {}

In[] : solve_poly(x**2-2*x+1,x,FiniteSet(2,3,4))
Out[] : {}

In[] : solve(x**2-2*x+1,x,FiniteSet(1,2,3,4))
Out[] : {1}

In[] : solve(x**2-3*x+2,x,Interval(0,100))
Out[] : {1,2}

```

#### Trigonometric Solvers 

For implementing trigonometric solvers, We need helpers for simplifying trigonometric expressions. Currently, SymEngine has a naive trig_simplify() which i am planning to expand on lines with the ```fu``` model in SymPy.

Consider the following situation - 
  If user's input equation is of form ``` 2*sin**2(x)+2*cos**2(x)+x-2 ```
  then instead of any generic algorithm for solving this equation, We can use the simple Identity ```sin**2(x)+cos**2(x) = 1```  and the equation now becomes deadly simple.

[Fu et al's algorithm](http://rfdz.ph-noe.ac.at/fileadmin/Mathematik_Uploads/ACDCA/DESTIME2006/DES_contribs/Fu/simplification.pdf) would be employed to simplify such equations.

In an Abstract way, This algorithm uses basic trigonometric transformations commonly stated in textbooks and expressed as TR1,TR2 and then simplifies the equation. Simplification of the generated expression tree is done in a bottom-up fashion.

Example..
```
TR7 -"Lowering the degree of cos(x)**2"

In[] : TR7(cos(x)**2)
Out[]: cos(2*x)/2 + 1/2

TR10i - "Sum of products to function of sum"

In[] : TR10i(sqrt(2)*cos(x)*x + sqrt(6)*sin(x)*x)
Out[] : 2*sqrt(2)*x*sin(x + pi/6)
```

After we simplify the equation using ```trig_simplify()```, the resultant equation is rewritten in terms of ```exp()``` by using conversion formulae described well [here](http://www.efunda.com/math/exp_log/exp2trig_hyper.cfm).
Next step is to expand the equation possibly a rational and substitute ```exp(I*x)``` with a dummy symbol say `x0` and solve the equation for the `x0` and resubsititute solutions of `x0` with ```exp(I*x)```. Now, We can find actual `x` easily by implementing an ```invert``` function that calculates its inverse . 

Similar to ```solve_poly```, ```solve_trig``` would be wrapped inside ```solve()```. 


Prototype :
```
template <typename Poly> RCP<const Set> solve_trig(const Poly& p,RCP<const Symbol> &x,
                              RCP<const Set> domain = Interval(-Inf,+Inf))
```

Examples : 
```
In[] : solve(tan(x)-1,x)
Out[] : {2*n*pi+5*pi/4|n in Z} U {2*n*pi+pi/4|n in Z}

In[] : solve(tan(x)-1,x,Interval(-pi/2,pi/2))
Out[] : {pi/4}

In[] : solve_trig(sin(x)**2+cos(x)**2-1,x)
Out[] : R

```
### Linear System of Equations : 
Solving system of linear equations is an important feature of solvers in a CAS, these arise quite frequently in "real life", problems and can be incredibly complex sometimes. SymPy has ```linsolve()``` for solving such equations. Multivariate Polynomials are currently supported for Integer coefficients only. I am planning to solve only those equations that are representable with this(Integral coefficients).<br><br>
**Approach** : 
- First the system of equations are converted into matrix form by implementing a new function ```lineareq_to_matrix``` that converts `N` linear equations with `M` variables into an augmented matrix comprising of  ```N*M``` sized matrix `A` amd ```N*1``` matrix `b`.

- Next is to Solve `Ax = b` using [Gauss Jordan elimination](http://pages.pacificcoast.net/~cazelais/251/gauss-jordan.pdf). Currently, this is implemented in ```SymEngine``` but it only supports square dense matrices. I will Overload this function to return output as ```Set``` and solve for both square and non-square matrices.  

- There may be zero, one, or infinite solutions.  If one solution exists, it will be returned. If infinite solutions exist, it will be returned parametrically. If no solutions exist, It will return ```EmptySet```, else it returns a ```FiniteSet``` of Solutions.

Prototype : 
```
RCP<const Set> guass_jordan_solve(const DenseMatrix &A,const DenseMatrix &b);

DenseMatrix lineareq_to_matrix(std::set<RCP<const MIntPoly>,RCPBasicKeyLess>);

RCP<const Set> linsolve(std::set<RCP<const MIntPoly>,RCPBasicKeyLess>eqns,std::set<RCP<const Symbol>,
                        RCPBasicKeyLess> symbols);

RCP<const Set> linsolve(const DenseMatrix aug,std::set<RCP<const Symbol>,
                        RCPBasicKeyLess> symbols);   #Overloading linsolve
```

Examples : 
```
auto A = DenseMatrix([[1, 1, 1], [2, 3, 5], [4, 0, 5]])
auto b = DenseMatrix([5, 8, 2])
In[] : guass_jordan_solve(A,b)
Out[] : [3,4,-2]

In[] : lineareq_to_matrix({3*x+2*y+z-5,x+3*y+5*z+9,4*x+3*y-5*z-2})
Out[] : DenseMatrix([3,2,1,-5],[1,3,5,9], [4,3,-5,-2])

auto aug = DenseMatrix([[1, 1, 1, 5], [2, 3, 5, 8], [4, 0, 5, 2]])
In[] : linsolve(aug,[x,y,z])
Out[] : {(3, 4, -2)}

In[] : linsolve({x+2*y -3*z-2,6*x+3*y-9*z-6,7*x+14*y-21*z-13},[x,y,z]}
Out[] : EmptySet()

```

### Applications of Solvers

Solvers have wide range of Applications. Some of them are addressed here.

**EigenVectors and EigenValues**  <br>
EigenVectors and EigenValues are frequently computed in Matrix Algebra. Currently SymEngine has a matrix module but due to lack of solvers, Eigenvectors couldn't be implemented. This issue is also raised [here](https://github.com/symengine/symengine/issues/268) by [Thilina](https://github.com/thilinarmtb). Computing them is easy and the algorithm is the [standard](http://math.mit.edu/~gs/linearalgebra/ila0601.pdf) text-book one.
Breifly , 
* Compute the determinant of `A - (lamda)*I` .It is a polynomial in `lamda` of degree atmost `n`.
* Find the roots of this polynomial, by solving `det(A - (lamda)*I = 0`. The `n` roots are
the `n` eigenvalues of `A`. They make `A - (lamda)*I` singular.
* For each eigenvalue , solve `(A - (lamda)*I)*x  = 0` to find an eigenvector `x`.


### Previous Implementation in SymPy
Sympy has a robust Solvers,polynomial and sets module.
[Harsh Gupta](https://github.com/hargup) started rewriting `solve` as `solve_set` in 2014 and several improvements took place since then.

Solvers module is implemented at `sympy.solvers` and majority of the work in my project would revolve specifically around algorithms implemented in `solveset.py`.

Trignometric Simplification is implemeted at `sympy.simplify` and `fu` algorithm is implemented at `fu.py`.

Sets module is implemented at `sympy.sets`.

#### Discussions
* SymEngine's [Mailing List](https://groups.google.com/forum/#!topic/symengine/vRKtOR_tnhI)

----------

# Timeline
I have my summer vacations starting from 1st May. I am already well-acquainted with the community.In between, I have no other commitments and will be easily able to give 40 - 50 hours a week.My college will restart in mid July but I will still be able to contribute atleast 35-40 hours since there will be no exams or tests until GSoC period ends.
I will also maintain a [blog](https://ranjithkumar007.github.io/) so that my mentors can regulate my work, and also monitor my progress.

#### Pre-GSoC
- I already went through Major Portion of SymPy's code for solvers.I would like to Inspect remaining part of it.  
- Discuss the choice and implementation of the structures and algorithms with mentors
- Atleast Write and verify Pseudo-Code for `fu` algorithm,`gcd_hueristic` and other complex algorithms so that i can go at full throttle in implementing them.

#### Week 1
- Implement ```gcd``` function in Integral domain.
- Implement ```lcm``` using ```gcd``` function

#### Week 2
- Implement ```zz_get_square_free_part()``` and ```zz_factor()```.

#### Week 3 & 4
- Implement the following in set module : 
  - `Complement`
  - `ConditionSet`
  - `ImageSet`
  
Before Phase 1 Evaluations,Set-Up for Solvers would be finished.

#### Week 5
Implement Polynomial solvers :
- Start working on Polynomial solvers by implementing solvers for lower degree polynomials(degree <= 4).
- Implement helper functions for cyclotomic polynomials like ```_inverse_totient```.

#### Week 6
- Implement solvers for cyclotomic polynomials.
- Integrate ```zz_factor()``` with ```solve_poly()```.
- Wrap up polynomial solvers into ```solve```.

#### Week 7, 8 & 9
Implementing trigonometric solvers.
- Implement ```fu``` algorithm for trigonometric expression simplication.
- Add ```rewrite_as_exp()``` for all trigonometric and hyperbolic functions
- Complete trigonometric solvers and wrap it in ```solve()```. 

Before Phase 2 Evaluations,polynomial solvers and major portion of trigonometric solvers would be completed.

#### Week 10 & 11
Implement System of Linear equations
- implement Guass-Jordan Elimination method for solving Matrices of the form Ax=b.
- implement ```lineareq_to_matrix``` function.
- implement ```linsolve()``` for solving system of equations. 
 
#### Week 12
- Implement some Applications of Solvers Like Finding EigenValues and EigenVectors
- Make report of all the work done on Solvers during the GSoC period.

#### Week 13
Buffer time.

#### Post GSoC

I would like to continue contributing to SymEngine and SymPy even after my GSoC period.
* I will start working on polynomials module after wrapping up my GSoC project. [Isuru](https://github.com/isuruf) already started making polynomials and series classes work with other Symbolic types.I would like to take up the remaining work as given in the TODO list [here](https://github.com/symengine/symengine/pull/1058).
----------


## References

* [Original Paper on Fu et al's algorithm](http://rfdz.ph-noe.ac.at/fileadmin/Mathematik_Uploads/ACDCA/DESTIME2006/DES_contribs/Fu/simplification.pdf)
* [Algorithm for Hueristic GCD](http://dl.acm.org/citation.cfm?doid=220346.220376)
* [CyclotomicPolynomials](http://mathworld.wolfram.com/CyclotomicPolynomial.html)
* [Harsh Gupta's GSoC Proposal](https://github.com/sympy/sympy/wiki/GSoC-2014-Application-Harsh-Gupta%3A-Solvers)
* [Amit Kumar GSoC Proposal](https://github.com/sympy/sympy/wiki/GSoC-2014-Application-Harsh-Gupta%3A-Solvers)
* [Solvers Wiki](https://github.com/sympy/sympy/wiki/solvers)
* [GSoC 2k16 Solvers Progress](https://github.com/sympy/sympy/wiki/GSoC-2016-Solvers-Progress) 
* [Fields in SymEngine](https://github.com/symengine/symengine/wiki/Fields-in-SymEngine)
* [Gauss Jordan elimination](http://pages.pacificcoast.net/~cazelais/251/gauss-jordan.pdf)
-----------
