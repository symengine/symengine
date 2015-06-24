from cython.operator cimport dereference as deref, preincrement as inc
cimport symengine
from symengine cimport rcp, RCP, set
from libcpp cimport bool
from libcpp.string cimport string
from cpython cimport PyObject, Py_XINCREF, Py_XDECREF, \
    PyObject_CallMethodObjArgs

class SympifyError(Exception):
    pass

cdef c2py(RCP[const symengine.Basic] o):
    cdef Basic r
    if (symengine.is_a_Add(deref(o))):
        r = Add.__new__(Add)
    elif (symengine.is_a_Mul(deref(o))):
        r = Mul.__new__(Mul)
    elif (symengine.is_a_Pow(deref(o))):
        r = Pow.__new__(Pow)
    elif (symengine.is_a_Integer(deref(o))):
        r = Integer.__new__(Integer)
    elif (symengine.is_a_Rational(deref(o))):
        r = Rational.__new__(Rational)
    elif (symengine.is_a_Complex(deref(o))):
        r = Complex.__new__(Complex)
    elif (symengine.is_a_Symbol(deref(o))):
        r = Symbol.__new__(Symbol)
    elif (symengine.is_a_Constant(deref(o))):
        r = Constant.__new__(Constant)
    elif (symengine.is_a_Sin(deref(o))):
        r = Sin.__new__(Sin)
    elif (symengine.is_a_Cos(deref(o))):
        r = Cos.__new__(Cos)
    elif (symengine.is_a_FunctionSymbol(deref(o))):
        r = FunctionSymbol.__new__(FunctionSymbol)
    elif (symengine.is_a_Abs(deref(o))):
        r = Abs.__new__(Abs)
    elif (symengine.is_a_Derivative(deref(o))):
        r = Derivative.__new__(Derivative)
    elif (symengine.is_a_Subs(deref(o))):
        r = Subs.__new__(Subs)
    elif (symengine.is_a_FunctionWrapper(deref(o))):
        r = FunctionWrapper.__new__(FunctionWrapper)
    elif (symengine.is_a_RealDouble(deref(o))):
        r = RealDouble.__new__(RealDouble)
    elif (symengine.is_a_ComplexDouble(deref(o))):
        r = ComplexDouble.__new__(ComplexDouble)
    else:
        raise Exception("Unsupported SymEngine class.")
    r.thisptr = o
    return r

def sympy2symengine(a, raise_error=False):
    """
    Converts 'a' from SymPy to SymEngine.

    If the expression cannot be converted, it either returns None (if
    raise_error==False) or raises a SympifyError exception (if
    raise_error==True).
    """
    import sympy
    from sympy.core.function import AppliedUndef as sympy_AppliedUndef
    if isinstance(a, sympy.Symbol):
        return Symbol(a.name)
    elif isinstance(a, sympy.Mul):
        x, y = a.as_two_terms()
        return sympy2symengine(x, True) * sympy2symengine(y, True)
    elif isinstance(a, sympy.Add):
        x, y = a.as_two_terms()
        return sympy2symengine(x, True) + sympy2symengine(y, True)
    elif isinstance(a, (sympy.Pow, sympy.exp)):
        x, y = a.as_base_exp()
        return sympy2symengine(x, True) ** sympy2symengine(y, True)
    elif isinstance(a, sympy.Integer):
        return Integer(a.p)
    elif isinstance(a, sympy.Rational):
        return Integer(a.p) / Integer(a.q)
    elif a is sympy.I:
        return I
    elif a is sympy.E:
        return E
    elif a is sympy.pi:
        return pi
    elif isinstance(a, sympy.sin):
        return sin(a.args[0])
    elif isinstance(a, sympy.cos):
        return cos(a.args[0])
    elif isinstance(a, sympy.Abs):
        return abs(sympy2symengine(a.args[0], True))
    elif isinstance(a, sympy.Derivative):
        return Derivative(a.expr, a.variables)
    elif isinstance(a, sympy.Subs):
        return Subs(a.expr, a.variables, a.point)
    elif isinstance(a, sympy_AppliedUndef):
        name = str(a.func)
        return function_symbol(name, *(a.args))
    elif isinstance(a, sympy.Function):
        return FunctionWrapper(a)
    elif isinstance(a, sympy.Matrix):
        row, col = a.shape
        v = []
        for r in a.tolist():
            for e in r:
                v.append(e)
        return DenseMatrix(row, col, v)
    elif isinstance(a, tuple):
        v = []
        for e in a:
            v.append(sympy2symengine(e, True))
        return tuple(v)
    elif isinstance(a, list):
        v = []
        for e in a:
            v.append(sympy2symengine(e, True))
        return v
    if raise_error:
        raise SympifyError("sympy2symengine: Cannot convert '%r' to a symengine type." % a)

def sympify(a, raise_error=True):
    if isinstance(a, (Basic, MatrixBase)):
        return a
    elif isinstance(a, (int, long)):
        return Integer(a)
    elif isinstance(a, float):
        return RealDouble(a)
    elif isinstance(a, complex):
        return ComplexDouble(a)
    else:
        return sympy2symengine(a, raise_error)

cdef class Basic(object):
    cdef RCP[const symengine.Basic] thisptr

    def __str__(self):
        return deref(self.thisptr).__str__().decode("utf-8")

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        return deref(self.thisptr).hash()

    def __add__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(symengine.add(A.thisptr, B.thisptr))

    def __sub__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(symengine.sub(A.thisptr, B.thisptr))

    def __mul__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(symengine.mul(A.thisptr, B.thisptr))

    def __truediv__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(symengine.div(A.thisptr, B.thisptr))

    # This is for Python 2.7 compatibility only:
    def __div__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(symengine.div(A.thisptr, B.thisptr))

    def __pow__(a, b, c):
        if c is not None:
            return powermod(a, b, c)
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(symengine.pow(A.thisptr, B.thisptr))

    def __neg__(Basic self not None):
        return c2py(symengine.neg(self.thisptr))

    def __abs__(Basic self not None):
        return c2py(symengine.abs(self.thisptr))

    def __richcmp__(a, b, int op):
        A = sympify(a, False)
        B = sympify(b, False)
        if not (isinstance(A, Basic) and isinstance(B, Basic)):
            if (op == 2):
                return False
            elif (op == 3):
                return True
            else:
                return NotImplemented
        return Basic._richcmp_(A, B, op)

    def _richcmp_(Basic A, Basic B, int op):
        if (op == 2):
            return symengine.eq(A.thisptr, B.thisptr)
        elif (op == 3):
            return symengine.neq(A.thisptr, B.thisptr)
        else:
            return NotImplemented

    def expand(Basic self not None):
        return c2py(symengine.expand(self.thisptr))

    def diff(Basic self not None, x):
        cdef Symbol symbol = sympify(x)
        cdef RCP[const symengine.Symbol] X = symengine.rcp_static_cast_Symbol(symbol.thisptr)
        return c2py(deref(self.thisptr).diff(X))

    def subs_dict(Basic self not None, subs_dict):
        cdef symengine.map_basic_basic d
        cdef Basic K, V
        for k in subs_dict:
            K = sympify(k)
            V = sympify(subs_dict[k])
            d[K.thisptr] = V.thisptr
        return c2py(deref(self.thisptr).subs(d))

    def subs_oldnew(Basic self not None, old, new):
        return self.subs_dict({old: new})

    def subs(Basic self not None, *args):
        if len(args) == 1:
            return self.subs_dict(args[0])
        elif len(args) == 2:
            return self.subs_oldnew(args[0], args[1])
        else:
            raise TypeError("subs() takes one or two arguments (%d given)" % \
                    len(args))

    @property
    def args(self):
        cdef symengine.vec_basic Y = deref(self.thisptr).get_args()
        s = []
        for i in range(Y.size()):
            s.append(c2py(<RCP[const symengine.Basic]>(Y[i])))
        return tuple(s)

    @property
    def free_symbols(self):
        cdef symengine.set_basic _set = symengine.free_symbols(deref(self.thisptr))
        return {c2py(<RCP[const symengine.Basic]>(elem)) for elem in _set}

cdef class Symbol(Basic):

    def __cinit__(self, name = None):
        if name is None:
            return
        self.thisptr = rcp(new symengine.Symbol(name.encode("utf-8")))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Symbol] X = symengine.rcp_static_cast_Symbol(self.thisptr)
        import sympy
        return sympy.Symbol(str(deref(X).get_name().decode("utf-8")))

cdef class Constant(Basic):

    def __cinit__(self, name = None):
        if name is None:
            return
        self.thisptr = rcp(new symengine.Constant(name.encode("utf-8")))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        import sympy
        if self == E:
            return sympy.E
        elif self == pi:
            return sympy.pi
        else:
            raise Exception("Unknown Constant")

cdef class Number(Basic):
    pass

cdef class Integer(Number):

    def __cinit__(self, i = None):
        if i is None:
            return
        cdef int i_
        cdef symengine.mpz_class i__
        cdef string tmp
        try:
            # Try to convert "i" to int
            i_ = i
            int_ok = True
        except OverflowError:
            # Too big, need to use mpz
            int_ok = False
            tmp = str(i).encode("utf-8")
            i__ = symengine.mpz_class(tmp, 10)
        # Note: all other exceptions are left intact
        if int_ok:
            self.thisptr = rcp(new symengine.Integer(i_))
        else:
            self.thisptr = rcp(new symengine.Integer(i__))

    def __dealloc__(self):
        self.thisptr.reset()

    def __hash__(self):
        return deref(self.thisptr).hash()

    def __richcmp__(a, b, int op):
        A = sympify(a, False)
        B = sympify(b, False)
        if not (isinstance(A, Integer) and isinstance(B, Integer)):
            if (op == 2):
                return False
            elif (op == 3):
                return True
            return NotImplemented
        return Integer._richcmp_(A, B, op)

    def _richcmp_(Integer A, Integer B, int op):
        cdef int i = deref(symengine.rcp_static_cast_Integer(A.thisptr)).compare(deref(symengine.rcp_static_cast_Integer(B.thisptr)))
        if (op == 0):
            return i < 0
        elif (op == 1):
            return i <= 0
        elif (op == 2):
            return i == 0
        elif (op == 3):
            return i != 0
        elif (op == 4):
            return i > 0
        elif (op == 5):
            return i >= 0
        else:
            return NotImplemented

    def __floordiv__(x, y):
        return quotient(x, y)

    def __mod__(x, y):
        return mod(x, y)

    def __divmod__(x, y):
        return quotient_mod(x, y)

    def _sympy_(self):
        import sympy
        return sympy.Integer(deref(self.thisptr).__str__().decode("utf-8"))


cdef class RealDouble(Number):

    def __cinit__(self, i = None):
        if i is None:
            return
        cdef double i_ = i
        self.thisptr = rcp(new symengine.RealDouble(i_))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        import sympy
        return sympy.Float(deref(self.thisptr).__str__().decode("utf-8"))

cdef class ComplexDouble(Number):

    def __cinit__(self, i = None):
        if i is None:
            return
        cdef double complex i_ = i
        self.thisptr = rcp(new symengine.ComplexDouble(i_))

    def __dealloc__(self):
        self.thisptr.reset()

    def real_part(self):
        return c2py(<RCP[const symengine.Basic]>deref(symengine.rcp_static_cast_ComplexDouble(self.thisptr)).real_part())

    def imaginary_part(self):
        return c2py(<RCP[const symengine.Basic]>deref(symengine.rcp_static_cast_ComplexDouble(self.thisptr)).imaginary_part())

    def _sympy_(self):
        import sympy
        return self.real_part()._sympy_() + sympy.I * self.imaginary_part()._sympy_()

cdef class Rational(Number):

    def __dealloc__(self):
        self.thisptr.reset()

    def get_num_den(self):
        cdef RCP[const symengine.Integer] _num, _den
        symengine.get_num_den(symengine.rcp_static_cast_Rational(self.thisptr),
                           symengine.outArg_Integer(_num), symengine.outArg_Integer(_den))
        return [c2py(<RCP[const symengine.Basic]>_num), c2py(<RCP[const symengine.Basic]>_den)]

    def _sympy_(self):
        rat = self.get_num_den()
        return rat[0]._sympy_() / rat[1]._sympy_()

cdef class Complex(Number):

    def __dealloc__(self):
        self.thisptr.reset()

    def real_part(self):
        return c2py(<RCP[const symengine.Basic]>deref(symengine.rcp_static_cast_Complex(self.thisptr)).real_part())

    def imaginary_part(self):
        return c2py(<RCP[const symengine.Basic]>deref(symengine.rcp_static_cast_Complex(self.thisptr)).imaginary_part())

    def _sympy_(self):
        import sympy
        return self.real_part()._sympy_() + sympy.I * self.imaginary_part()._sympy_()

cdef class Add(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Add] X = symengine.rcp_static_cast_Add(self.thisptr)
        cdef RCP[const symengine.Basic] a, b
        deref(X).as_two_terms(symengine.outArg(a), symengine.outArg(b))
        return c2py(a)._sympy_() + c2py(b)._sympy_()

cdef class Mul(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Mul] X = symengine.rcp_static_cast_Mul(self.thisptr)
        cdef RCP[const symengine.Basic] a, b
        deref(X).as_two_terms(symengine.outArg(a), symengine.outArg(b))
        return c2py(a)._sympy_() * c2py(b)._sympy_()

cdef class Pow(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Pow] X = symengine.rcp_static_cast_Pow(self.thisptr)
        base = c2py(deref(X).get_base())
        exp = c2py(deref(X).get_exp())
        return base._sympy_() ** exp._sympy_()

cdef class Function(Basic):
    pass

cdef class Sin(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Sin] X = symengine.rcp_static_cast_Sin(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.sin(arg)

cdef class Cos(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Cos] X = symengine.rcp_static_cast_Cos(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.cos(arg)

cdef class FunctionSymbol(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.FunctionSymbol] X = \
            symengine.rcp_static_cast_FunctionSymbol(self.thisptr)
        name = deref(X).get_name().decode("utf-8")
        # In Python 2.7, function names cannot be unicode:
        name = str(name)
        cdef symengine.vec_basic Y = deref(X).get_args()
        s = []
        for i in range(Y.size()):
            s.append(c2py(<RCP[const symengine.Basic]>(Y[i]))._sympy_())
        import sympy
        return sympy.Function(name)(*s)

cdef inline void SymPy_XDECREF(void* o):
    Py_XDECREF(<PyObject*>o)

cdef inline void SymPy_XINCREF(void* o):
    Py_XINCREF(<PyObject*>o)

cdef inline int SymPy_CMP(void* o1, void* o2):
    return <int>PyObject_CallMethodObjArgs(<object>o1, "compare", <PyObject *>o2, NULL)

cdef class FunctionWrapper(FunctionSymbol):

    def __cinit__(self, sympy_function = None):
        import sympy
        if not isinstance(sympy_function, sympy.Function):
            return
        cdef void* ptr
        ptr = <void *>(sympy_function)
        cdef string name = str(sympy_function.func).encode("utf-8")
        cdef string hash_ = str(sympy_function.__hash__()).encode("utf-8")
        cdef symengine.vec_basic v
        cdef Basic arg_
        for arg in sympy_function.args:
            arg_ = sympify(arg)
            v.push_back(arg_.thisptr)
        SymPy_XINCREF(ptr)
        self.thisptr = rcp(new symengine.FunctionWrapper(ptr, name, hash_, v, &SymPy_XDECREF, &SymPy_CMP))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef object pyobj
        cdef RCP[const symengine.FunctionWrapper] X = \
            symengine.rcp_static_cast_FunctionWrapper(self.thisptr)
        pyobj = <object>(deref(X).get_object())
        return pyobj

cdef class Abs(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Abs] X = symengine.rcp_static_cast_Abs(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        return abs(arg)


cdef class Derivative(Basic):

    def __cinit__(self, expr = None, symbols = None):
        if expr is None or symbols is None:
            return
        cdef symengine.vec_basic vec
        cdef Basic s_
        cdef Basic expr_ = sympify(expr, True)
        for s in symbols:
            s_ = sympify(s, True)
            vec.push_back(s_.thisptr)
        self.thisptr = rcp(new symengine.Derivative(<const RCP[const symengine.Basic]>expr_.thisptr, vec))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Derivative] X = \
            symengine.rcp_static_cast_Derivative(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        cdef symengine.vec_basic Y = deref(X).get_symbols()
        s = []
        for i in range(Y.size()):
            s.append(c2py(<RCP[const symengine.Basic]>(Y[i]))._sympy_())
        import sympy
        return sympy.Derivative(arg, *s)

cdef class Subs(Basic):

    def __cinit__(self, expr = None, variables = None, point = None):
        if expr is None or variables is None or point is None:
            return
        cdef symengine.map_basic_basic m
        cdef Basic v_
        cdef Basic p_
        cdef Basic expr_ = sympify(expr, True)
        for v, p in zip(variables, point):
            v_ = sympify(v, True)
            p_ = sympify(p, True)
            m[v_.thisptr] = p_.thisptr
        self.thisptr = rcp(new symengine.Subs(<const RCP[const symengine.Basic]>expr_.thisptr, m))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const symengine.Subs] X = symengine.rcp_static_cast_Subs(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        cdef symengine.vec_basic V = deref(X).get_variables()
        cdef symengine.vec_basic P = deref(X).get_point()
        v = []
        p = []
        for i in range(V.size()):
            v.append(c2py(<RCP[const symengine.Basic]>(V[i]))._sympy_())
            p.append(c2py(<RCP[const symengine.Basic]>(P[i]))._sympy_())
        import sympy
        return sympy.Subs(arg, v, p)

cdef class MatrixBase:
    cdef symengine.MatrixBase* thisptr

    def __richcmp__(a, b, int op):
        A = sympify(a, False)
        B = sympify(b, False)
        if not (isinstance(A, MatrixBase) and isinstance(B, MatrixBase)):
            if (op == 2):
                return False
            elif (op == 3):
                return True
            return NotImplemented
        return MatrixBase._richcmp_(A, B, op)

    def _richcmp_(MatrixBase A, MatrixBase B, int op):
        if (op == 2):
            return deref(A.thisptr).eq(deref(B.thisptr))
        elif (op == 3):
            return not deref(A.thisptr).eq(deref(B.thisptr))
        else:
            return NotImplemented

cdef class DenseMatrix(MatrixBase):

    def __cinit__(self, row, col):
        self.thisptr = new symengine.DenseMatrix(row, col)

    def __cinit__(self, row, col, v):
        cdef symengine.vec_basic v_
        cdef Basic e_
        for e in v:
            e_ = sympify(e, False)
            if e_ is not None:
                v_.push_back(e_.thisptr)

        self.thisptr = new symengine.DenseMatrix(row, col, v_)

    def __str__(self):
        return deref(self.thisptr).__str__().decode("utf-8")

    def __dealloc__(self):
        del self.thisptr

    def nrows(self):
        return deref(self.thisptr).nrows()

    def ncols(self):
        return deref(self.thisptr).ncols()

    def get(self, i, j):
        # No error checking is done
        return c2py(deref(self.thisptr).get(i, j))

    def set(self, i, j, e):
        cdef Basic e_ = sympify(e)
        if e_ is not None:
            deref(self.thisptr).set(i, j, <const RCP[const symengine.Basic] &>(e_.thisptr))

    def det(self):
        return c2py(deref(self.thisptr).det())

    def inv(self, method='LU'):
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())

        if method.upper() == 'LU':
            ## inv() method of DenseMatrix uses LU factorization
            deref(self.thisptr).inv(deref(result.thisptr))
        elif method.upper() == 'FFLU':
            symengine.inverse_FFLU(deref(symengine.static_cast_DenseMatrix(self.thisptr)),
                deref(symengine.static_cast_DenseMatrix(result.thisptr)))
        elif method.upper() == 'GJ':
            symengine.inverse_GJ(deref(symengine.static_cast_DenseMatrix(self.thisptr)),
                deref(symengine.static_cast_DenseMatrix(result.thisptr)))
        else:
            raise Exception("Unsupported method.")
        return result

    def add_matrix(self, A):
        cdef MatrixBase A_ = sympify(A)
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).add_matrix(deref(A_.thisptr), deref(result.thisptr))
        return result

    def mul_matrix(self, A):
        cdef MatrixBase A_ = sympify(A)
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).mul_matrix(deref(A_.thisptr), deref(result.thisptr))
        return result

    def add_scalar(self, k):
        cdef Basic k_ = sympify(k)
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).add_scalar(<const RCP[const symengine.Basic] &>(k_.thisptr), deref(result.thisptr))
        return result

    def mul_scalar(self, k):
        cdef Basic k_ = sympify(k)
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).mul_scalar(<const RCP[const symengine.Basic] &>(k_.thisptr), deref(result.thisptr))
        return result

    def transpose(self):
        result = DenseMatrix(self.ncols(), self.nrows(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).transpose(deref(result.thisptr))
        return result

    def submatrix(self, r_i, r_j, c_i, c_j):
        result = DenseMatrix(r_j - r_i + 1, c_j - c_i + 1, [0]*(r_j - r_i + 1)*(c_j - c_i + 1))
        deref(self.thisptr).submatrix(r_i, r_j, c_i, c_j, deref(result.thisptr))
        return result

    def LU(self):
        L = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        U = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).LU(deref(L.thisptr), deref(U.thisptr))
        return L, U

    def LDL(self):
        L = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        D = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).LDL(deref(L.thisptr), deref(D.thisptr))
        return L, D

    def solve(self, b, method='LU'):
        cdef DenseMatrix b_ = sympify(b)
        x = DenseMatrix(b_.nrows(), b_.ncols(), [0]*b_.nrows()*b_.ncols())

        if method.upper() == 'LU':
            ## solve() method of DenseMatrix uses LU factorization
            deref(self.thisptr).LU_solve(deref(b_.thisptr), deref(x.thisptr))
        elif method.upper() == 'FFLU':
            symengine.FFLU_solve(deref(symengine.static_cast_DenseMatrix(self.thisptr)),
                deref(symengine.static_cast_DenseMatrix(b_.thisptr)),
                deref(symengine.static_cast_DenseMatrix(x.thisptr)))
        elif method.upper() == 'LDL':
            symengine.LDL_solve(deref(symengine.static_cast_DenseMatrix(self.thisptr)),
                deref(symengine.static_cast_DenseMatrix(b_.thisptr)),
                deref(symengine.static_cast_DenseMatrix(x.thisptr)))
        elif method.upper() == 'FFGJ':
            symengine.FFGJ_solve(deref(symengine.static_cast_DenseMatrix(self.thisptr)),
                deref(symengine.static_cast_DenseMatrix(b_.thisptr)),
                deref(symengine.static_cast_DenseMatrix(x.thisptr)))
        else:
            raise Exception("Unsupported method.")

        return x

    def FFLU(self):
        L = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        U = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).FFLU(deref(L.thisptr))

        for i in range(self.nrows()):
            for j in range(i + 1, self.ncols()):
                U.set(i, j, L.get(i, j))
                L.set(i, j, 0)
            U.set(i, i, L.get(i, i))

        return L, U

    def FFLDU(self):
        L = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        D = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        U = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).FFLDU(deref(L.thisptr), deref(D.thisptr), deref(U.thisptr))
        return L, D, U

    def _sympy_(self):
        s = []
        cdef symengine.DenseMatrix A = deref(symengine.static_cast_DenseMatrix(self.thisptr))
        for i in range(A.nrows()):
            l = []
            for j in range(A.ncols()):
                l.append(c2py(A.get(i, j))._sympy_())
            s.append(l)
        import sympy
        return sympy.Matrix(s)

cdef class Sieve:
    @staticmethod
    def generate_primes(n):
        cdef symengine.vector[unsigned] primes
        symengine.sieve_generate_primes(primes, n)
        s = []
        for i in range(primes.size()):
            s.append(primes[i])
        return s

cdef class Sieve_iterator:
    cdef symengine.sieve_iterator *thisptr
    cdef unsigned limit
    def __cinit__(self):
        self.thisptr = new symengine.sieve_iterator()
        self.limit = 0

    def __cinit__(self, n):
        self.thisptr = new symengine.sieve_iterator(n)
        self.limit = n

    def __iter__(self):
        return self

    def __next__(self):
        n = deref(self.thisptr).next_prime()
        if self.limit > 0 and n > self.limit:
            raise StopIteration
        else:
            return n

def sin(x):
    cdef Basic X = sympify(x)
    return c2py(symengine.sin(X.thisptr))

def cos(x):
    cdef Basic X = sympify(x)
    return c2py(symengine.cos(X.thisptr))

def function_symbol(name, *args):
    cdef symengine.vec_basic v
    cdef Basic e_
    for e in args:
        e_ = sympify(e, False)
        if e_ is not None:
            v.push_back(e_.thisptr)
    return c2py(symengine.function_symbol(name.encode("utf-8"), v))

def sqrt(x):
    cdef Basic X = sympify(x)
    return c2py(symengine.sqrt(X.thisptr))

def exp(x):
    cdef Basic X = sympify(x)
    return c2py(symengine.exp(X.thisptr))

def densematrix(row, col, l):
    return DenseMatrix(row, col, l)

def probab_prime_p(n, reps = 25):
    cdef Integer _n = sympify(n)
    return symengine.probab_prime_p(deref(symengine.rcp_static_cast_Integer(_n.thisptr)), reps) >= 1

def nextprime(n):
    cdef Integer _n = sympify(n)
    return c2py(<RCP[const symengine.Basic]>(symengine.nextprime(deref(symengine.rcp_static_cast_Integer(_n.thisptr)))))

def gcd(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const symengine.Basic]>(symengine.gcd(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_b.thisptr)))))

def lcm(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const symengine.Basic]>(symengine.lcm(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_b.thisptr)))))

def gcd_ext(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    cdef RCP[const symengine.Integer] g, s, t
    symengine.gcd_ext(symengine.outArg_Integer(g), symengine.outArg_Integer(s), symengine.outArg_Integer(t),
        deref(symengine.rcp_static_cast_Integer(_a.thisptr)), deref(symengine.rcp_static_cast_Integer(_b.thisptr)))
    return [c2py(<RCP[const symengine.Basic]>g), c2py(<RCP[const symengine.Basic]>s), c2py(<RCP[const symengine.Basic]>t)]

def mod(a, b):
    if b == 0:
        raise ZeroDivisionError
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const symengine.Basic]>(symengine.mod(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_b.thisptr)))))

def quotient(a, b):
    if b == 0:
        raise ZeroDivisionError
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const symengine.Basic]>(symengine.quotient(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_b.thisptr)))))

def quotient_mod(a, b):
    if b == 0:
        raise ZeroDivisionError
    cdef RCP[const symengine.Integer] q, r
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    symengine.quotient_mod(symengine.outArg_Integer(q), symengine.outArg_Integer(r),
        deref(symengine.rcp_static_cast_Integer(_a.thisptr)), deref(symengine.rcp_static_cast_Integer(_b.thisptr)))
    return (c2py(<RCP[const symengine.Basic]>q), c2py(<RCP[const symengine.Basic]>r))

def mod_inverse(a, b):
    cdef RCP[const symengine.Integer] inv
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    cdef int ret_val = symengine.mod_inverse(symengine.outArg_Integer(inv),
        deref(symengine.rcp_static_cast_Integer(_a.thisptr)), deref(symengine.rcp_static_cast_Integer(_b.thisptr)))
    if ret_val == 0:
        return None
    return c2py(<RCP[const symengine.Basic]>inv)

def crt(rem, mod):
    cdef symengine.vec_integer _rem, _mod
    cdef Integer _a
    cdef bool ret_val
    for i in range(len(rem)):
        _a = sympify(rem[i])
        _rem.push_back(symengine.rcp_static_cast_Integer(_a.thisptr))
        _a = sympify(mod[i])
        _mod.push_back(symengine.rcp_static_cast_Integer(_a.thisptr))

    cdef RCP[const symengine.Integer] c
    ret_val = symengine.crt(symengine.outArg_Integer(c), _rem, _mod)
    if not ret_val:
        return None
    return c2py(<RCP[const symengine.Basic]>c)

def fibonacci(n):
    if n < 0 :
        raise NotImplementedError
    return c2py(<RCP[const symengine.Basic]>(symengine.fibonacci(n)))

def fibonacci2(n):
    if n < 0 :
        raise NotImplementedError
    cdef RCP[const symengine.Integer] f1, f2
    symengine.fibonacci2(symengine.outArg_Integer(f1), symengine.outArg_Integer(f2), n)
    return [c2py(<RCP[const symengine.Basic]>f1), c2py(<RCP[const symengine.Basic]>f2)]

def lucas(n):
    if n < 0 :
        raise NotImplementedError
    return c2py(<RCP[const symengine.Basic]>(symengine.lucas(n)))

def lucas2(n):
    if n < 0 :
        raise NotImplementedError
    cdef RCP[const symengine.Integer] f1, f2
    symengine.lucas2(symengine.outArg_Integer(f1), symengine.outArg_Integer(f2), n)
    return [c2py(<RCP[const symengine.Basic]>f1), c2py(<RCP[const symengine.Basic]>f2)]

def binomial(n, k):
    if k < 0:
        raise ArithmeticError
    cdef Integer _n = sympify(n)
    return c2py(<RCP[const symengine.Basic]>symengine.binomial(deref(symengine.rcp_static_cast_Integer(_n.thisptr)), k))

def factorial(n):
    if n < 0:
        raise ArithmeticError
    return c2py(<RCP[const symengine.Basic]>(symengine.factorial(n)))

def divides(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return symengine.divides(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_b.thisptr)))

def factor(n, B1 = 1.0):
    cdef Integer _n = sympify(n)
    cdef RCP[const symengine.Integer] f
    cdef int ret_val = symengine.factor(symengine.outArg_Integer(f),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)), B1)
    if (ret_val == 1):
        return c2py(<RCP[const symengine.Basic]>f)
    else:
        return None

def factor_lehman_method(n):
    cdef Integer _n = sympify(n)
    cdef RCP[const symengine.Integer] f
    cdef int ret_val = symengine.factor_lehman_method(symengine.outArg_Integer(f),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)))
    if (ret_val == 1):
        return c2py(<RCP[const symengine.Basic]>f)
    else:
        return None

def factor_pollard_pm1_method(n, B = 10, retries = 5):
    cdef Integer _n = sympify(n)
    cdef RCP[const symengine.Integer] f
    cdef int ret_val = symengine.factor_pollard_pm1_method(symengine.outArg_Integer(f),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)), B, retries)
    if (ret_val == 1):
        return c2py(<RCP[const symengine.Basic]>f)
    else:
        return None

def factor_pollard_rho_method(n, retries = 5):
    cdef Integer _n = sympify(n)
    cdef RCP[const symengine.Integer] f
    cdef int ret_val = symengine.factor_pollard_rho_method(symengine.outArg_Integer(f),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)), retries)
    if (ret_val == 1):
        return c2py(<RCP[const symengine.Basic]>f)
    else:
        return None

def prime_factors(n):
    cdef symengine.vec_integer factors
    cdef Integer _n = sympify(n)
    symengine.prime_factors(factors, deref(symengine.rcp_static_cast_Integer(_n.thisptr)))
    s = []
    for i in range(factors.size()):
        s.append(c2py(<RCP[const symengine.Basic]>(factors[i])))
    return s

def prime_factor_multiplicities(n):
    cdef symengine.vec_integer factors
    cdef Integer _n = sympify(n)
    symengine.prime_factors(factors, deref(symengine.rcp_static_cast_Integer(_n.thisptr)))
    cdef Basic r
    dict = {}
    for i in range(factors.size()):
        r = c2py(<RCP[const symengine.Basic]>(factors[i]))
        if (r not in dict):
            dict[r] = 1
        else:
            dict[r] += 1
    return dict

def bernoulli(n):
    if n < 0:
        raise ArithmeticError
    return c2py(<RCP[const symengine.Basic]>(symengine.bernoulli(n)))

def primitive_root(n):
    cdef RCP[const symengine.Integer] g
    cdef Integer _n = sympify(n)
    cdef bool ret_val = symengine.primitive_root(symengine.outArg_Integer(g),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)))
    if ret_val == 0:
        return None
    return c2py(<RCP[const symengine.Basic]>g)

def primitive_root_list(n):
    cdef symengine.vec_integer root_list
    cdef Integer _n = sympify(n)
    symengine.primitive_root_list(root_list,
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)))
    s = []
    for i in range(root_list.size()):
        s.append(c2py(<RCP[const symengine.Basic]>(root_list[i])))
    return s

def totient(n):
    cdef Integer _n = sympify(n)
    cdef RCP[const symengine.Integer] m = symengine.rcp_static_cast_Integer(_n.thisptr)
    return c2py(<RCP[const symengine.Basic]>symengine.totient(<const RCP[const symengine.Integer]>m))

def carmichael(n):
    cdef Integer _n = sympify(n)
    cdef RCP[const symengine.Integer] m = symengine.rcp_static_cast_Integer(_n.thisptr)
    return c2py(<RCP[const symengine.Basic]>symengine.carmichael(<const RCP[const symengine.Integer]>m))

def multiplicative_order(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    cdef RCP[const symengine.Integer] n1 = symengine.rcp_static_cast_Integer(_n.thisptr)
    cdef RCP[const symengine.Integer] a1 = symengine.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const symengine.Integer] o
    cdef bool c = symengine.multiplicative_order(symengine.outArg_Integer(o),
        <const RCP[const symengine.Integer]>a1, <const RCP[const symengine.Integer]>n1)
    if not c:
        return None
    return c2py(<RCP[const symengine.Basic]>o)

def legendre(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    return symengine.legendre(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)))

def jacobi(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    return symengine.jacobi(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)))

def kronecker(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    return symengine.kronecker(deref(symengine.rcp_static_cast_Integer(_a.thisptr)),
        deref(symengine.rcp_static_cast_Integer(_n.thisptr)))

def nthroot_mod(a, n, m):
    cdef RCP[const symengine.Integer] root
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef RCP[const symengine.Integer] n1 = symengine.rcp_static_cast_Integer(_n.thisptr)
    cdef RCP[const symengine.Integer] a1 = symengine.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const symengine.Integer] m1 = symengine.rcp_static_cast_Integer(_m.thisptr)
    cdef bool ret_val = symengine.nthroot_mod(symengine.outArg_Integer(root),
        <const RCP[const symengine.Integer]>a1, <const RCP[const symengine.Integer]>n1,
        <const RCP[const symengine.Integer]>m1)
    if not ret_val:
        return None
    return c2py(<RCP[const symengine.Basic]>root)

def nthroot_mod_list(a, n, m):
    cdef symengine.vec_integer root_list
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef RCP[const symengine.Integer] n1 = symengine.rcp_static_cast_Integer(_n.thisptr)
    cdef RCP[const symengine.Integer] a1 = symengine.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const symengine.Integer] m1 = symengine.rcp_static_cast_Integer(_m.thisptr)
    symengine.nthroot_mod_list(root_list, <const RCP[const symengine.Integer]>a1,
        <const RCP[const symengine.Integer]>n1, <const RCP[const symengine.Integer]>m1)
    s = []
    for i in range(root_list.size()):
        s.append(c2py(<RCP[const symengine.Basic]>(root_list[i])))
    return s

def powermod(a, b, m):
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef Number _b = sympify(b)
    cdef RCP[const symengine.Integer] a1 = symengine.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const symengine.Integer] m1 = symengine.rcp_static_cast_Integer(_m.thisptr)
    cdef RCP[const symengine.Number] b1 = symengine.rcp_static_cast_Number(_b.thisptr)
    cdef RCP[const symengine.Integer] root

    cdef bool ret_val = symengine.powermod(symengine.outArg_Integer(root),
        <const RCP[const symengine.Integer]>a1, <const RCP[const symengine.Number]>b1,
        <const RCP[const symengine.Integer]>m1)
    if ret_val == 0:
        return None
    return c2py(<RCP[const symengine.Basic]>root)

def powermod_list(a, b, m):
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef Number _b = sympify(b)
    cdef RCP[const symengine.Integer] a1 = symengine.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const symengine.Integer] m1 = symengine.rcp_static_cast_Integer(_m.thisptr)
    cdef RCP[const symengine.Number] b1 = symengine.rcp_static_cast_Number(_b.thisptr)
    cdef symengine.vec_integer v

    symengine.powermod_list(v,
        <const RCP[const symengine.Integer]>a1, <const RCP[const symengine.Number]>b1,
        <const RCP[const symengine.Integer]>m1)
    s = []
    for i in range(v.size()):
        s.append(c2py(<RCP[const symengine.Basic]>(v[i])))
    return s

def eval_double(basic):
    cdef Basic b = sympify(basic)
    return symengine.eval_double(deref(b.thisptr))

I = c2py(symengine.I)
E = c2py(symengine.E)
pi = c2py(symengine.pi)

# Turn on nice stacktraces:
symengine.print_stack_on_segfault()
