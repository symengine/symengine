import os
import matchpy
from sympy.integrals.rubi.utility_function import *
from sympy.integrals.rubi.symbol import WC
from cpp_code_generation import CppCodeGenerator
from matchpy.matching.code_generation import CodeGenerator
from symengine_printer import symengine_print
from sympy import *

x, y, z = symbols("x y z")
a, b, c = symbols("a b c")
f = Function("f")
w = WC("w", commutative=True)


GENERATED_TEST_DIR = "autogen_tests"


collection_of_expressions = [
        ([x], {x: (0, {}), y: None}),
        ([x**y], {x**y: (0, {}), x**z: None}),
        ([x**y, w], {x**y: (0, {}), x: (1, {w: x}), x+y: (1, {w: x+y})}),
        ([x+y, x**2], {y+x: (0, {}), x**2: (1, {}), x**3: None}),
        ([x**w,], {y+x: None, x**2: (0, {}), x**3: (0, {})}),
]

def generate_matchpy_matcher(pattern_list):
    matcher = matchpy.ManyToOneMatcher()
    for pattern in pattern_list:
        matcher.add(matchpy.Pattern(pattern))
    return matcher


def generate_cpp_code(matcher):
    cg = CppCodeGenerator(matcher)
    a, b = cg.generate_code()
    return a, b


def generate_python_code(matcher):
    cg = CodeGenerator(matcher)
    a, b = cg.generate_code()
    return a, b


def export_code_to_file(filename, a, b, test_number):
    patterns = collection_of_expressions[test_number][0]
    patterns = [str(i) for i in patterns]
    fout = open(os.path.join(GENERATED_TEST_DIR, filename), "w")
    fout.write("""\
/*
 * This file was automatically generated: DO NOT EDIT.
 *
 * Decision tree matching expressions {0}
 */
""".format(str(patterns)))
    fout.write('#include "catch.hpp"\n')
    fout.write(a)
    fout.write("\n\n")
    fout.write(b)
    return fout


def add_main_with_tests(fout, test_cases, test_number):
    fout.write("""
TEST_CASE("GeneratedMatchPyTest{0}", "")
{{
    generator<tuple<int, Substitution>> ret;
    Substitution substitution;
""".format(test_number))
    pattern_list = collection_of_expressions[test_number][0]
    for test_case, result in test_cases.items():
        fout.write("\n")
        if result is not None:
            pattern_id, substitution = result
            substitution_pretty = {symengine_print(k): str(v) for k, v in substitution.items()}
            fout.write("    // Pattern {0} matching {1} with substitution {2}:\n".format(pattern_list[pattern_id], test_case, substitution_pretty))
            fout.write("    ret = match_root({0});\n".format(symengine_print(test_case)))
            fout.write("    substitution = get<1>(ret[0]);\n")
            fout.write("    REQUIRE(ret.size() > 0);\n")
            fout.write("    REQUIRE(get<0>(ret[0]) == {0});\n".format(pattern_id))
            for key, value in substitution.items():
                wildcard = symengine_print(key)
                matched = symengine_print(value)
                #import pdb; pdb.set_trace()
                fout.write('    REQUIRE(substitution.find("{0}") != substitution.end());\n'.format(wildcard, matched))
                fout.write('    REQUIRE((*substitution.at("{0}").begin())->__eq__(*{1}));\n'.format(wildcard, matched))
        else:
            fout.write("    // Pattern {0} not matching:\n".format(test_case))
            fout.write("    ret = match_root({0});\n".format(symengine_print(test_case)))
            fout.write("    REQUIRE(ret.size() == 0);\n")
    fout.write("}\n")


def generate_tests():
    fout = open(os.path.join(GENERATED_TEST_DIR, "CMakeLists.txt"), "w")
    fout.write("""\
project(matchpycpp_tests)

include_directories(BEFORE ${symengine_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR})
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

include_directories(BEFORE ${catch_SOURCE_DIR})
""")
    for i, (pattern_list, test_cases) in enumerate(collection_of_expressions):
        matcher = generate_matchpy_matcher(pattern_list)
        a, b = generate_cpp_code(matcher)
        filename = "test_case{0:03}".format(i+1)
        filenamecpp = "{}.cpp".format(filename)
        fwrite = export_code_to_file(filenamecpp, a, b, i)
        add_main_with_tests(fwrite, test_cases, i)
        fwrite.close()
        fout.write("\n")
        fout.write("add_executable({0} {1})\n".format(filename, filenamecpp))
        fout.write("target_link_libraries({0} symengine catch)\n".format(filename))
        #fout.write("target_link_libraries({0} matchpycpp)\n".format(filename))

        filenamepy = "{}.py".format(filename)
        a, b = generate_python_code(matcher)
        fpy = open(os.path.join(GENERATED_TEST_DIR, filenamepy), "w")
        fpy.write(a)
        fpy.write("\n\n")
        fpy.write(b)
        fpy.close()

    fout.close()


if __name__ == "__main__":
    generate_tests()
