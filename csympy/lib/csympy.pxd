from libcpp.string cimport string

cdef extern from "basic.h" namespace "Teuchos":
    cdef enum ENull:
        null

    cdef cppclass RCP[T]:
        T& operator*() nogil except +
        # Not yet supported in Cython:
#        RCP[T]& operator=(RCP[T] &r_ptr) nogil except +
        void reset() nogil except +


cdef extern from "basic.h" namespace "CSymPy":
    cdef cppclass Basic:
        string __str__() nogil except +


cdef extern from "symbol.h" namespace "CSymPy":
    cdef cppclass Symbol(Basic):
        Symbol(string name) nogil
#        string __str__() nogil except +


cdef extern from "integer.h" namespace "CSymPy":
    cdef cppclass Integer(Basic):
        Integer(int i) nogil
#        string __str__() nogil except +


cdef extern from "add.h" namespace "CSymPy":
    cdef RCP[Basic] add(RCP[Basic] &a, RCP[Basic] &b) nogil except+


cdef extern from "basic.h" namespace "Teuchos":
    # We need to specialize these for our classes:
    cdef RCP[Symbol] rcp(Symbol *p)
    cdef RCP[Integer] rcp(Integer *p)

