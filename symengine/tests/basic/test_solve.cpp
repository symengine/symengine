// #include <symengine/solve.h>
// #include <symengine/mul.h>
// #include <symengine/add.h>

// using SymEngine::solve;
// using SymEngine::RCP;
// using SymEngine::Basic;
// using SymEngine::integer;
// using SymEngine::rational;
// using SymEngine::add;
// using SymEngine::symbol;
// using SymEngine::emptyset;


// TEST_CASE("test_solve", "[Solve]")
// {
//     RCP<const Symbol> x = symbol("x");
//     RCP<const Basic> poly = add(x,integer(3));
//     RCP<const Interval> reals = interval(NegInf,Inf,true,true);
//     RCP<const Set> soln;
    
//     soln = solve(poly,x,reals);
//     REQUIRE(*soln,*finiteset({integer(-3)}));

//     poly = add(mul(integer(2),x),integer(3));
//     soln = solve(poly,x,reals);
//     REQUIRE(*soln,*finiteset({rational(-3,2)}));

//     soln = solve(poly,x,interval(zero,Inf,false,true));
//     REQUIRE(*soln,*emptyset());
// }      