from libcpp.string cimport string

cdef extern from "symbol.h" namespace "CSymPy":

    cdef cppclass Symbol:
        Symbol(string name)
        string __str__()
