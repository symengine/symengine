from libcpp.string cimport string

cdef extern from "symbol.h" namespace "CSymPy":

    cdef cppclass Symbol:
        Symbol(string name)
        string __str__()

    cdef cppclass Integer:
        Integer(int i)
        string __str__()

cdef extern from "basic.h" namespace "Teuchos":

    cdef cppclass RCP[T]:
        T& operator*() nogil except +

    cdef RCP[Symbol] rcp(Symbol *p)
    cdef RCP[Integer] rcp(Integer *p)
