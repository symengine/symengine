from libcpp.string cimport string

cdef extern from "symbol.h" namespace "CSymPy":

    cdef cppclass Symbol:
        Symbol(string name)
        string __str__()

cdef extern from "integer.h" namespace "CSymPy":

    cdef cppclass Integer:
        Integer(int i)
        string __str__()

cdef extern from "basic.h" namespace "Teuchos":
    cdef enum ENull:
        null

    cdef cppclass RCP[T]:
        T& operator*() nogil except +
        # Not yet supported in Cython:
#        RCP[T]& operator=(RCP[T] &r_ptr) nogil except +
        void reset() nogil except +

    # We need to specialize these for our classes:
    cdef RCP[Symbol] rcp(Symbol *p)
    cdef RCP[Integer] rcp(Integer *p)

