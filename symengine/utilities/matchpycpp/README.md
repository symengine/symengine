

# Usage example

Clone MatchPy and checkout some past commit (the current wrapper in SymPy is not compatible with the latest MatchPy commit):

```
git clone https://github.com/HPAC/matchpy
cd matchpy
git checkout 419c103
```

Make sure that MatchPy is in the `PYTHONPATH`.

Import the MatchPy wrapper from SymPy:

```
from sympy.integrals.rubi.utility_function import *
from sympy.integrals.rubi.symbol import WC
```

Note: the `utility_function` module needs to be imported as the link between SymPy and MatchPy expressions is defined there.

Import MatchPy and create a many-to-one matcher object:

```
from matchpy import *

matcher = ManyToOneMatcher()
```

Define SymPy variables and a MatchPy-SymPy wilcard `w`:

```
w = WC("w")
x, y, z = symbols("x y z")
```

Add some expressions to the many-to-one matcher:

```
matcher.add(Pattern(x+y))
matcher.add(Pattern(2**w))
```

Use MatchPy to match an expression:

```
>>> list(matcher.match(2**z))
[(Pattern(2**w), {'w': z})]
```

which means that pattern `2**w` matches the expression `2**z` with substitution `{'w': z}`.

The C++ code performing the same matching can be generated with:

```
from cpp_code_generation import CppCodeGenerator
cg = CppCodeGenerator(matcher)
part1, part2 = cg.generate_code()
```

Now write `part1` and `part2` to a file:

```
fout = open("sample_matching_test.cpp", "w")
fout.write(part1)
fout.write(part2)
fout.close()
```
