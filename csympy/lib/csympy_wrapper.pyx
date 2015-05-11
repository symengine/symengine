from cython.operator cimport dereference as deref, preincrement as inc
cimport csympy
from csympy cimport rcp, RCP, set
from libcpp cimport bool
from libcpp.string cimport string
from cpython cimport PyObject, Py_XINCREF, Py_XDECREF, \
    PyObject_CallMethodObjArgs

class SympifyError(Exception):
    pass

cdef c2py(RCP[const csympy.Basic] o):
    cdef Basic r
    if (csympy.is_a_Add(deref(o))):
        r = Add.__new__(Add)
    elif (csympy.is_a_Mul(deref(o))):
        r = Mul.__new__(Mul)
    elif (csympy.is_a_Pow(deref(o))):
        r = Pow.__new__(Pow)
    elif (csympy.is_a_Integer(deref(o))):
        r = Integer.__new__(Integer)
    elif (csympy.is_a_Rational(deref(o))):
        r = Rational.__new__(Rational)
    elif (csympy.is_a_Complex(deref(o))):
        r = Complex.__new__(Complex)
    elif (csympy.is_a_Symbol(deref(o))):
        r = Symbol.__new__(Symbol)
    elif (csympy.is_a_Constant(deref(o))):
        r = Constant.__new__(Constant)
    elif (csympy.is_a_Sin(deref(o))):
        r = Sin.__new__(Sin)
    elif (csympy.is_a_Cos(deref(o))):
        r = Cos.__new__(Cos)
    elif (csympy.is_a_FunctionSymbol(deref(o))):
        r = FunctionSymbol.__new__(FunctionSymbol)
    elif (csympy.is_a_Abs(deref(o))):
        r = Abs.__new__(Abs)
    elif (csympy.is_a_Derivative(deref(o))):
        r = Derivative.__new__(Derivative)
    elif (csympy.is_a_Subs(deref(o))):
        r = Subs.__new__(Subs)
    elif (csympy.is_a_FunctionWrapper(deref(o))):
        r = FunctionWrapper.__new__(FunctionWrapper)
    else:
        raise Exception("Unsupported CSymPy class.")
    r.thisptr = o
    return r

def sympy2csympy(a, raise_error=False):
    """
    Converts 'a' from SymPy to CSymPy.

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
        return sympy2csympy(x, True) * sympy2csympy(y, True)
    elif isinstance(a, sympy.Add):
        x, y = a.as_two_terms()
        return sympy2csympy(x, True) + sympy2csympy(y, True)
    elif isinstance(a, (sympy.Pow, sympy.exp)):
        x, y = a.as_base_exp()
        return sympy2csympy(x, True) ** sympy2csympy(y, True)
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
        return abs(sympy2csympy(a.args[0], True))
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
            v.append(sympy2csympy(e, True))
        return tuple(v)
    elif isinstance(a, list):
        v = []
        for e in a:
            v.append(sympy2csympy(e, True))
        return v
    if raise_error:
        raise SympifyError("sympy2csympy: Cannot convert '%r' to a csympy type." % a)

def sympify(a, raise_error=True):
    if isinstance(a, (Basic, MatrixBase)):
        return a
    elif isinstance(a, (int, long)):
        return Integer(a)
    else:
        return sympy2csympy(a, raise_error)

cdef class Basic(object):
    cdef RCP[const csympy.Basic] thisptr

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
        return c2py(csympy.add(A.thisptr, B.thisptr))

    def __sub__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.sub(A.thisptr, B.thisptr))

    def __mul__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.mul(A.thisptr, B.thisptr))

    def __truediv__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.div(A.thisptr, B.thisptr))

    # This is for Python 2.7 compatibility only:
    def __div__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.div(A.thisptr, B.thisptr))

    def __pow__(a, b, c):
        if c is not None:
            return powermod(a, b, c)
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.pow(A.thisptr, B.thisptr))

    def __neg__(Basic self not None):
        return c2py(csympy.neg(self.thisptr))

    def __abs__(Basic self not None):
        return c2py(csympy.abs(self.thisptr))

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
            return csympy.eq(A.thisptr, B.thisptr)
        elif (op == 3):
            return csympy.neq(A.thisptr, B.thisptr)
        else:
            return NotImplemented

    def expand(Basic self not None):
        return c2py(csympy.expand(self.thisptr))

    def diff(Basic self not None, x):
        cdef Symbol symbol = sympify(x)
        cdef RCP[const csympy.Symbol] X = csympy.rcp_static_cast_Symbol(symbol.thisptr)
        return c2py(deref(self.thisptr).diff(X))

    def subs_dict(Basic self not None, subs_dict):
        cdef csympy.map_basic_basic d
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
        cdef csympy.vec_basic Y = deref(self.thisptr).get_args()
        s = []
        for i in range(Y.size()):
            s.append(c2py(<RCP[const csympy.Basic]>(Y[i])))
        return tuple(s)

    @property
    def free_symbols(self):
        cdef csympy.set_basic _set = csympy.free_symbols(deref(self.thisptr))
        return {c2py(<RCP[const csympy.Basic]>(elem)) for elem in _set}

cdef class Symbol(Basic):

    def __cinit__(self, name = None):
        if name is None:
            return
        self.thisptr = rcp(new csympy.Symbol(name.encode("utf-8")))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Symbol] X = csympy.rcp_static_cast_Symbol(self.thisptr)
        import sympy
        return sympy.Symbol(str(deref(X).get_name().decode("utf-8")))

cdef class Constant(Basic):

    def __cinit__(self, name = None):
        if name is None:
            return
        self.thisptr = rcp(new csympy.Constant(name.encode("utf-8")))

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
        cdef csympy.mpz_class i__
        cdef string tmp
        try:
            # Try to convert "i" to int
            i_ = i
            int_ok = True
        except OverflowError:
            # Too big, need to use mpz
            int_ok = False
            tmp = str(i).encode("utf-8")
            i__ = csympy.mpz_class(tmp, 10)
        # Note: all other exceptions are left intact
        if int_ok:
            self.thisptr = rcp(new csympy.Integer(i_))
        else:
            self.thisptr = rcp(new csympy.Integer(i__))

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
        cdef int i = deref(csympy.rcp_static_cast_Integer(A.thisptr)).compare(deref(csympy.rcp_static_cast_Integer(B.thisptr)))
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

cdef class Rational(Number):

    def __dealloc__(self):
        self.thisptr.reset()

    def get_num_den(self):
        cdef RCP[const csympy.Integer] _num, _den
        csympy.get_num_den(csympy.rcp_static_cast_Rational(self.thisptr),
                           csympy.outArg_Integer(_num), csympy.outArg_Integer(_den))
        return [c2py(<RCP[const csympy.Basic]>_num), c2py(<RCP[const csympy.Basic]>_den)]

    def _sympy_(self):
        rat = self.get_num_den()
        return rat[0]._sympy_() / rat[1]._sympy_()

cdef class Complex(Number):

    def __dealloc__(self):
        self.thisptr.reset()

    def real_part(self):
        return c2py(<RCP[const csympy.Basic]>deref(csympy.rcp_static_cast_Complex(self.thisptr)).real_part())

    def imaginary_part(self):
        return c2py(<RCP[const csympy.Basic]>deref(csympy.rcp_static_cast_Complex(self.thisptr)).imaginary_part())

    def _sympy_(self):
        import sympy
        return self.real_part()._sympy_() + sympy.I * self.imaginary_part()._sympy_()

cdef class Add(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Add] X = csympy.rcp_static_cast_Add(self.thisptr)
        cdef RCP[const csympy.Basic] a, b
        deref(X).as_two_terms(csympy.outArg(a), csympy.outArg(b))
        return c2py(a)._sympy_() + c2py(b)._sympy_()

cdef class Mul(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Mul] X = csympy.rcp_static_cast_Mul(self.thisptr)
        cdef RCP[const csympy.Basic] a, b
        deref(X).as_two_terms(csympy.outArg(a), csympy.outArg(b))
        return c2py(a)._sympy_() * c2py(b)._sympy_()

cdef class Pow(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Pow] X = csympy.rcp_static_cast_Pow(self.thisptr)
        base = c2py(deref(X).get_base())
        exp = c2py(deref(X).get_exp())
        return base._sympy_() ** exp._sympy_()

cdef class Function(Basic):
    pass

cdef class Sin(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Sin] X = csympy.rcp_static_cast_Sin(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.sin(arg)

cdef class Cos(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Cos] X = csympy.rcp_static_cast_Cos(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.cos(arg)

cdef class FunctionSymbol(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.FunctionSymbol] X = \
            csympy.rcp_static_cast_FunctionSymbol(self.thisptr)
        name = deref(X).get_name().decode("utf-8")
        # In Python 2.7, function names cannot be unicode:
        name = str(name)
        cdef csympy.vec_basic Y = deref(X).get_args()
        s = []
        for i in range(Y.size()):
            s.append(c2py(<RCP[const csympy.Basic]>(Y[i]))._sympy_())
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
        cdef csympy.vec_basic v
        cdef Basic arg_
        for arg in sympy_function.args:
            arg_ = sympify(arg)
            v.push_back(arg_.thisptr)
        SymPy_XINCREF(ptr)
        self.thisptr = rcp(new csympy.FunctionWrapper(ptr, name, hash_, v, &SymPy_XDECREF, &SymPy_CMP))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef object pyobj
        cdef RCP[const csympy.FunctionWrapper] X = \
            csympy.rcp_static_cast_FunctionWrapper(self.thisptr)
        pyobj = <object>(deref(X).get_object())
        return pyobj

cdef class Abs(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Abs] X = csympy.rcp_static_cast_Abs(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        return abs(arg)


cdef class Derivative(Basic):

    def __cinit__(self, expr = None, symbols = None):
        if expr is None or symbols is None:
            return
        cdef csympy.vec_basic vec
        cdef Basic s_
        cdef Basic expr_ = sympify(expr, True)
        for s in symbols:
            s_ = sympify(s, True)
            vec.push_back(s_.thisptr)
        self.thisptr = rcp(new csympy.Derivative(<const RCP[const csympy.Basic]>expr_.thisptr, vec))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Derivative] X = \
            csympy.rcp_static_cast_Derivative(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        cdef csympy.vec_basic Y = deref(X).get_symbols()
        s = []
        for i in range(Y.size()):
            s.append(c2py(<RCP[const csympy.Basic]>(Y[i]))._sympy_())
        import sympy
        return sympy.Derivative(arg, *s)

cdef class Subs(Basic):

    def __cinit__(self, expr = None, variables = None, point = None):
        if expr is None or variables is None or point is None:
            return
        cdef csympy.map_basic_basic m
        cdef Basic v_
        cdef Basic p_
        cdef Basic expr_ = sympify(expr, True)
        for v, p in zip(variables, point):
            v_ = sympify(v, True)
            p_ = sympify(p, True)
            m[v_.thisptr] = p_.thisptr
        self.thisptr = rcp(new csympy.Subs(<const RCP[const csympy.Basic]>expr_.thisptr, m))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Subs] X = csympy.rcp_static_cast_Subs(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        cdef csympy.vec_basic V = deref(X).get_variables()
        cdef csympy.vec_basic P = deref(X).get_point()
        v = []
        p = []
        for i in range(V.size()):
            v.append(c2py(<RCP[const csympy.Basic]>(V[i]))._sympy_())
            p.append(c2py(<RCP[const csympy.Basic]>(P[i]))._sympy_())
        import sympy
        return sympy.Subs(arg, v, p)

cdef class MatrixBase:
    cdef csympy.MatrixBase* thisptr

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
        self.thisptr = new csympy.DenseMatrix(row, col)

    def __cinit__(self, row, col, v):
        cdef csympy.vec_basic v_
        cdef Basic e_
        for e in v:
            e_ = sympify(e, False)
            if e_ is not None:
                v_.push_back(e_.thisptr)

        self.thisptr = new csympy.DenseMatrix(row, col, v_)

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
            deref(self.thisptr).set(i, j, <const RCP[const csympy.Basic] &>(e_.thisptr))

    def det(self):
        return c2py(deref(self.thisptr).det())

    def inv(self, method='LU'):
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())

        if method.upper() == 'LU':
            ## inv() method of DenseMatrix uses LU factorization
            deref(self.thisptr).inv(deref(result.thisptr))
        elif method.upper() == 'FFLU':
            csympy.inverse_FFLU(deref(csympy.static_cast_DenseMatrix(self.thisptr)),
                deref(csympy.static_cast_DenseMatrix(result.thisptr)))
        elif method.upper() == 'GJ':
            csympy.inverse_GJ(deref(csympy.static_cast_DenseMatrix(self.thisptr)),
                deref(csympy.static_cast_DenseMatrix(result.thisptr)))
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
        deref(self.thisptr).add_scalar(<const RCP[const csympy.Basic] &>(k_.thisptr), deref(result.thisptr))
        return result

    def mul_scalar(self, k):
        cdef Basic k_ = sympify(k)
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).mul_scalar(<const RCP[const csympy.Basic] &>(k_.thisptr), deref(result.thisptr))
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
            csympy.FFLU_solve(deref(csympy.static_cast_DenseMatrix(self.thisptr)),
                deref(csympy.static_cast_DenseMatrix(b_.thisptr)),
                deref(csympy.static_cast_DenseMatrix(x.thisptr)))
        elif method.upper() == 'LDL':
            csympy.LDL_solve(deref(csympy.static_cast_DenseMatrix(self.thisptr)),
                deref(csympy.static_cast_DenseMatrix(b_.thisptr)),
                deref(csympy.static_cast_DenseMatrix(x.thisptr)))
        elif method.upper() == 'FFGJ':
            csympy.FFGJ_solve(deref(csympy.static_cast_DenseMatrix(self.thisptr)),
                deref(csympy.static_cast_DenseMatrix(b_.thisptr)),
                deref(csympy.static_cast_DenseMatrix(x.thisptr)))
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
        cdef csympy.DenseMatrix A = deref(csympy.static_cast_DenseMatrix(self.thisptr))
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
        cdef csympy.vector[unsigned] primes
        csympy.sieve_generate_primes(primes, n)
        s = []
        for i in range(primes.size()):
            s.append(primes[i])
        return s

cdef class Sieve_iterator:
    cdef csympy.sieve_iterator *thisptr
    cdef unsigned limit
    def __cinit__(self):
        self.thisptr = new csympy.sieve_iterator()
        self.limit = 0

    def __cinit__(self, n):
        self.thisptr = new csympy.sieve_iterator(n)
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
    return c2py(csympy.sin(X.thisptr))

def cos(x):
    cdef Basic X = sympify(x)
    return c2py(csympy.cos(X.thisptr))

def function_symbol(name, *args):
    cdef csympy.vec_basic v
    cdef Basic e_
    for e in args:
        e_ = sympify(e, False)
        if e_ is not None:
            v.push_back(e_.thisptr)
    return c2py(csympy.function_symbol(name.encode("utf-8"), v))

def sqrt(x):
    cdef Basic X = sympify(x)
    return c2py(csympy.sqrt(X.thisptr))

def exp(x):
    cdef Basic X = sympify(x)
    return c2py(csympy.exp(X.thisptr))

def densematrix(row, col, l):
    return DenseMatrix(row, col, l)

def probab_prime_p(n, reps = 25):
    cdef Integer _n = sympify(n)
    return csympy.probab_prime_p(deref(csympy.rcp_static_cast_Integer(_n.thisptr)), reps) >= 1

def nextprime(n):
    cdef Integer _n = sympify(n)
    return c2py(<RCP[const csympy.Basic]>(csympy.nextprime(deref(csympy.rcp_static_cast_Integer(_n.thisptr)))))

def gcd(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const csympy.Basic]>(csympy.gcd(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_b.thisptr)))))

def lcm(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const csympy.Basic]>(csympy.lcm(deref(csympy.rcp_static_cast_Integer(_a.thisptr)), 
        deref(csympy.rcp_static_cast_Integer(_b.thisptr)))))

def gcd_ext(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    cdef RCP[const csympy.Integer] g, s, t
    csympy.gcd_ext(csympy.outArg_Integer(g), csympy.outArg_Integer(s), csympy.outArg_Integer(t),
        deref(csympy.rcp_static_cast_Integer(_a.thisptr)), deref(csympy.rcp_static_cast_Integer(_b.thisptr)))
    return [c2py(<RCP[const csympy.Basic]>g), c2py(<RCP[const csympy.Basic]>s), c2py(<RCP[const csympy.Basic]>t)]

def mod(a, b):
    if b == 0:
        raise ZeroDivisionError
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const csympy.Basic]>(csympy.mod(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_b.thisptr)))))

def quotient(a, b):
    if b == 0:
        raise ZeroDivisionError
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return c2py(<RCP[const csympy.Basic]>(csympy.quotient(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_b.thisptr)))))

def quotient_mod(a, b):
    if b == 0:
        raise ZeroDivisionError
    cdef RCP[const csympy.Integer] q, r
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    csympy.quotient_mod(csympy.outArg_Integer(q), csympy.outArg_Integer(r),
        deref(csympy.rcp_static_cast_Integer(_a.thisptr)), deref(csympy.rcp_static_cast_Integer(_b.thisptr)))
    return (c2py(<RCP[const csympy.Basic]>q), c2py(<RCP[const csympy.Basic]>r))

def mod_inverse(a, b):
    cdef RCP[const csympy.Integer] inv
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    cdef int ret_val = csympy.mod_inverse(csympy.outArg_Integer(inv),
        deref(csympy.rcp_static_cast_Integer(_a.thisptr)), deref(csympy.rcp_static_cast_Integer(_b.thisptr)))
    if ret_val == 0:
        return None
    return c2py(<RCP[const csympy.Basic]>inv)

def crt(rem, mod):
    cdef csympy.vec_integer _rem, _mod
    cdef Integer _a
    cdef bool ret_val
    for i in range(len(rem)):
        _a = sympify(rem[i])
        _rem.push_back(csympy.rcp_static_cast_Integer(_a.thisptr))
        _a = sympify(mod[i])
        _mod.push_back(csympy.rcp_static_cast_Integer(_a.thisptr))

    cdef RCP[const csympy.Integer] c
    ret_val = csympy.crt(csympy.outArg_Integer(c), _rem, _mod)
    if not ret_val:
        return None
    return c2py(<RCP[const csympy.Basic]>c)

def fibonacci(n):
    if n < 0 :
        raise NotImplementedError
    return c2py(<RCP[const csympy.Basic]>(csympy.fibonacci(n)))

def fibonacci2(n):
    if n < 0 :
        raise NotImplementedError
    cdef RCP[const csympy.Integer] f1, f2
    csympy.fibonacci2(csympy.outArg_Integer(f1), csympy.outArg_Integer(f2), n)
    return [c2py(<RCP[const csympy.Basic]>f1), c2py(<RCP[const csympy.Basic]>f2)]

def lucas(n):
    if n < 0 :
        raise NotImplementedError
    return c2py(<RCP[const csympy.Basic]>(csympy.lucas(n)))

def lucas2(n):
    if n < 0 :
        raise NotImplementedError
    cdef RCP[const csympy.Integer] f1, f2
    csympy.lucas2(csympy.outArg_Integer(f1), csympy.outArg_Integer(f2), n)
    return [c2py(<RCP[const csympy.Basic]>f1), c2py(<RCP[const csympy.Basic]>f2)]

def binomial(n, k):
    if k < 0:
        raise ArithmeticError
    cdef Integer _n = sympify(n)
    return c2py(<RCP[const csympy.Basic]>csympy.binomial(deref(csympy.rcp_static_cast_Integer(_n.thisptr)), k))

def factorial(n):
    if n < 0:
        raise ArithmeticError
    return c2py(<RCP[const csympy.Basic]>(csympy.factorial(n)))

def divides(a, b):
    cdef Integer _a = sympify(a)
    cdef Integer _b = sympify(b)
    return csympy.divides(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_b.thisptr)))

def factor(n, B1 = 1.0):
    cdef Integer _n = sympify(n)
    cdef RCP[const csympy.Integer] f
    cdef int ret_val = csympy.factor(csympy.outArg_Integer(f),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)), B1)
    if (ret_val == 1):
        return c2py(<RCP[const csympy.Basic]>f)
    else:
        return None

def factor_lehman_method(n):
    cdef Integer _n = sympify(n)
    cdef RCP[const csympy.Integer] f
    cdef int ret_val = csympy.factor_lehman_method(csympy.outArg_Integer(f),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)))
    if (ret_val == 1):
        return c2py(<RCP[const csympy.Basic]>f)
    else:
        return None

def factor_pollard_pm1_method(n, B = 10, retries = 5):
    cdef Integer _n = sympify(n)
    cdef RCP[const csympy.Integer] f
    cdef int ret_val = csympy.factor_pollard_pm1_method(csympy.outArg_Integer(f),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)), B, retries)
    if (ret_val == 1):
        return c2py(<RCP[const csympy.Basic]>f)
    else:
        return None

def factor_pollard_rho_method(n, retries = 5):
    cdef Integer _n = sympify(n)
    cdef RCP[const csympy.Integer] f
    cdef int ret_val = csympy.factor_pollard_rho_method(csympy.outArg_Integer(f),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)), retries)
    if (ret_val == 1):
        return c2py(<RCP[const csympy.Basic]>f)
    else:
        return None

def prime_factors(n):
    cdef csympy.vec_integer factors
    cdef Integer _n = sympify(n)
    csympy.prime_factors(factors, deref(csympy.rcp_static_cast_Integer(_n.thisptr)))
    s = []
    for i in range(factors.size()):
        s.append(c2py(<RCP[const csympy.Basic]>(factors[i])))
    return s

def prime_factor_multiplicities(n):
    cdef csympy.vec_integer factors
    cdef Integer _n = sympify(n)
    csympy.prime_factors(factors, deref(csympy.rcp_static_cast_Integer(_n.thisptr)))
    cdef Basic r
    dict = {}
    for i in range(factors.size()):
        r = c2py(<RCP[const csympy.Basic]>(factors[i]))
        if (r not in dict):
            dict[r] = 1
        else:
            dict[r] += 1
    return dict

def bernoulli(n):
    if n < 0:
        raise ArithmeticError
    return c2py(<RCP[const csympy.Basic]>(csympy.bernoulli(n)))

def primitive_root(n):
    cdef RCP[const csympy.Integer] g
    cdef Integer _n = sympify(n)
    cdef bool ret_val = csympy.primitive_root(csympy.outArg_Integer(g),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)))
    if ret_val == 0:
        return None
    return c2py(<RCP[const csympy.Basic]>g)

def primitive_root_list(n):
    cdef csympy.vec_integer root_list
    cdef Integer _n = sympify(n)
    csympy.primitive_root_list(root_list,
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)))
    s = []
    for i in range(root_list.size()):
        s.append(c2py(<RCP[const csympy.Basic]>(root_list[i])))
    return s

def totient(n):
    cdef Integer _n = sympify(n)
    cdef RCP[const csympy.Integer] m = csympy.rcp_static_cast_Integer(_n.thisptr)
    return c2py(<RCP[const csympy.Basic]>csympy.totient(<const RCP[const csympy.Integer]>m))

def carmichael(n):
    cdef Integer _n = sympify(n)
    cdef RCP[const csympy.Integer] m = csympy.rcp_static_cast_Integer(_n.thisptr)
    return c2py(<RCP[const csympy.Basic]>csympy.carmichael(<const RCP[const csympy.Integer]>m))

def multiplicative_order(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    cdef RCP[const csympy.Integer] n1 = csympy.rcp_static_cast_Integer(_n.thisptr)
    cdef RCP[const csympy.Integer] a1 = csympy.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const csympy.Integer] o
    cdef bool c = csympy.multiplicative_order(csympy.outArg_Integer(o),
        <const RCP[const csympy.Integer]>a1, <const RCP[const csympy.Integer]>n1)
    if not c:
        return None
    return c2py(<RCP[const csympy.Basic]>o)

def legendre(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    return csympy.legendre(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)))

def jacobi(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    return csympy.jacobi(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)))

def kronecker(a, n):
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    return csympy.kronecker(deref(csympy.rcp_static_cast_Integer(_a.thisptr)),
        deref(csympy.rcp_static_cast_Integer(_n.thisptr)))

def nthroot_mod(a, n, m):
    cdef RCP[const csympy.Integer] root
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef RCP[const csympy.Integer] n1 = csympy.rcp_static_cast_Integer(_n.thisptr)
    cdef RCP[const csympy.Integer] a1 = csympy.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const csympy.Integer] m1 = csympy.rcp_static_cast_Integer(_m.thisptr)
    cdef bool ret_val = csympy.nthroot_mod(csympy.outArg_Integer(root),
        <const RCP[const csympy.Integer]>a1, <const RCP[const csympy.Integer]>n1,
        <const RCP[const csympy.Integer]>m1)
    if not ret_val:
        return None
    return c2py(<RCP[const csympy.Basic]>root)

def nthroot_mod_list(a, n, m):
    cdef csympy.vec_integer root_list
    cdef Integer _n = sympify(n)
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef RCP[const csympy.Integer] n1 = csympy.rcp_static_cast_Integer(_n.thisptr)
    cdef RCP[const csympy.Integer] a1 = csympy.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const csympy.Integer] m1 = csympy.rcp_static_cast_Integer(_m.thisptr)
    csympy.nthroot_mod_list(root_list, <const RCP[const csympy.Integer]>a1,
        <const RCP[const csympy.Integer]>n1, <const RCP[const csympy.Integer]>m1)
    s = []
    for i in range(root_list.size()):
        s.append(c2py(<RCP[const csympy.Basic]>(root_list[i])))
    return s

def powermod(a, b, m):
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef Number _b = sympify(b)
    cdef RCP[const csympy.Integer] a1 = csympy.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const csympy.Integer] m1 = csympy.rcp_static_cast_Integer(_m.thisptr)
    cdef RCP[const csympy.Number] b1 = csympy.rcp_static_cast_Number(_b.thisptr)
    cdef RCP[const csympy.Integer] root

    cdef bool ret_val = csympy.powermod(csympy.outArg_Integer(root),
        <const RCP[const csympy.Integer]>a1, <const RCP[const csympy.Number]>b1,
        <const RCP[const csympy.Integer]>m1)
    if ret_val == 0:
        return None
    return c2py(<RCP[const csympy.Basic]>root)

def powermod_list(a, b, m):
    cdef Integer _a = sympify(a)
    cdef Integer _m = sympify(m)
    cdef Number _b = sympify(b)
    cdef RCP[const csympy.Integer] a1 = csympy.rcp_static_cast_Integer(_a.thisptr)
    cdef RCP[const csympy.Integer] m1 = csympy.rcp_static_cast_Integer(_m.thisptr)
    cdef RCP[const csympy.Number] b1 = csympy.rcp_static_cast_Number(_b.thisptr)
    cdef csympy.vec_integer v

    csympy.powermod_list(v,
        <const RCP[const csympy.Integer]>a1, <const RCP[const csympy.Number]>b1,
        <const RCP[const csympy.Integer]>m1)
    s = []
    for i in range(v.size()):
        s.append(c2py(<RCP[const csympy.Basic]>(v[i])))
    return s

def eval_double(basic):
    cdef Basic b = sympify(basic)
    return csympy.eval_double(deref(b.thisptr))

I = c2py(csympy.I)
E = c2py(csympy.E)
pi = c2py(csympy.pi)

# Turn on nice stacktraces:
csympy.print_stack_on_segfault()
