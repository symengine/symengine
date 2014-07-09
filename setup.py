from __future__ import print_function
from distutils.core import setup
from distutils.command.build_py import build_py as _build_py
import subprocess

def cmake_build():
    if subprocess.call(["cmake", ".", "-DWITH_PYTHON=yes"]) != 0:
        raise EnvironmentError("error calling cmake")
            
    if subprocess.call("make") != 0:
        raise EnvironmentError("error calling make")

class BuildWithCmake(_build_py):
    def run(self):
        cmake_build()
        # can't use super() here because _build_py is an old style class in 2.7
        _build_py.run(self)

long_description = '''
CSymPy is a standalone fast C++ symbolic manipulation library. 
Optional thin Python wrappers allow easy usage from Python and 
integration with SymPy.'''

setup(name = "csympy", 
      version = "git",
      description = "CSymPy is a standalone fast C++ symbolic manipulation library",
      long_description = "",
      author = "",
      author_email = "",
      license = "MIT",
      url = "https://github.com/sympy/csympy",
      packages = ["csympy", "csympy.lib", "csympy.tests"],
      package_data= {'' : ['lib/csympy_wrapper.so']},
      cmdclass={
          'build_py' : BuildWithCmake,
          }
  )
