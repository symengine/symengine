from cython.operator cimport dereference as deref
cimport csympy
from csympy cimport rcp, RCP
from libcpp.string cimport string

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
        # TODO: figure out how to bypass the __init__() completely:
        r = Integer.__new__(Integer, -99999)
    elif (csympy.is_a_Rational(deref(o))):
        r = Rational.__new__(Rational)
    elif (csympy.is_a_Complex(deref(o))):
        r = Complex.__new__(Complex)
    elif (csympy.is_a_Symbol(deref(o))):
        # TODO: figure out how to bypass the __init__() completely:
        r = Symbol.__new__(Symbol, "null")
    elif (csympy.is_a_Sin(deref(o))):
        r = Sin.__new__(Sin)
    elif (csympy.is_a_Cos(deref(o))):
        r = Cos.__new__(Cos)
    elif (csympy.is_a_FunctionSymbol(deref(o))):
        r = FunctionSymbol.__new__(FunctionSymbol)
    elif (csympy.is_a_Derivative(deref(o))):
        r = Derivative.__new__(Derivative)
    else:
        raise Exception("Unsupported CSymPy class.")
    r.thisptr = o
    return r

def sympy2csympy(a, raise_error=False):
    """
    Converts 'a' from SymPy to CSymPy.

    Returns None if the expression cannot be converted.
    """
    import sympy
    if isinstance(a, sympy.Symbol):
        return Symbol(a.name)
    elif isinstance(a, sympy.Mul):
        x, y = a.as_two_terms()
        return sympy2csympy(x, True) * sympy2csympy(y, True)
    elif isinstance(a, sympy.Add):
        x, y = a.as_two_terms()
        return sympy2csympy(x, True) + sympy2csympy(y, True)
    elif isinstance(a, sympy.Pow):
        x, y = a.as_base_exp()
        return sympy2csympy(x, True) ** sympy2csympy(y, True)
    elif isinstance(a, sympy.Integer):
        return Integer(a.p)
    elif isinstance(a, sympy.Rational):
        return Integer(a.p) / Integer(a.q)
    elif a is sympy.I:
        return I
    elif isinstance(a, sympy.sin):
        return sin(a.args[0])
    elif isinstance(a, sympy.cos):
        return cos(a.args[0])
    elif isinstance(a, sympy.Function):
        name = str(a.func)
        arg = a.args[0]
        return function_symbol(name, sympy2csympy(arg, True))
    elif isinstance(a, sympy.Matrix):
        row, col = a.shape
        v = []
        for r in a.tolist():
            for e in r:
                v.append(e)
        return DenseMatrix(row, col, v)
    if raise_error:
        raise SympifyError("sympy2csympy: Cannot convert '%r' to a csympy type." % a)

def sympify(a, raise_error=True):
    if isinstance(a, (Basic, MatrixBase)):
        return a
    elif isinstance(a, (int, long)):
        return Integer(a)
    else:
        try:
            e = sympy2csympy(a)
            if e is not None:
                return e
        except ImportError:
            pass

        if raise_error:
            raise SympifyError("Cannot convert '%r' to a csympy type." % a)

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

    # What is the purpose of "c" here?
    def __pow__(a, b, c):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.pow(A.thisptr, B.thisptr))

    def __neg__(Basic self not None):
        return c2py(csympy.neg(self.thisptr))

    def __richcmp__(a, b, int op):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        if (op == 2):
            return csympy.eq(A.thisptr, B.thisptr)
        elif (op == 3):
            return csympy.neq(A.thisptr, B.thisptr)
        else:
            return NotImplemented

    def expand(Basic self not None):
        return c2py(csympy.expand(self.thisptr))

    def diff(Basic self not None, Symbol x not None):
        cdef RCP[const csympy.Symbol] X = csympy.rcp_static_cast_Symbol(x.thisptr)
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


cdef class Symbol(Basic):

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Symbol(name.encode("utf-8")))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[const csympy.Symbol] X = csympy.rcp_static_cast_Symbol(self.thisptr)
        import sympy
        return sympy.Symbol(str(deref(X).get_name().decode("utf-8")))

cdef class Number(Basic):
    pass

cdef class Integer(Number):

    def __cinit__(self, i):
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

    def _sympy_(self):
        import sympy
        return sympy.Integer(deref(self.thisptr).__str__().decode("utf-8"))

cdef class Rational(Number):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        import sympy
        return sympy.Rational(deref(self.thisptr).__str__().decode("utf-8"))

cdef class Complex(Number):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        import sympy
        # FIXME: this is quite fragile. We should request the real and
        # imaginary parts and construct the sympy expression using those (and
        # using sympy.I explicitly), rather than relying on the string
        # representation and hoping sympy.sympify() will do the right thing.
        s = deref(self.thisptr).__str__().decode("utf-8")
        return sympy.sympify(s)

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
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.Function(name)(arg)

cdef class Derivative(Basic):

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

cdef class MatrixBase:
    cdef csympy.MatrixBase* thisptr

    def __richcmp__(a, b, int op):
        cdef MatrixBase A = sympify(a, False)
        cdef MatrixBase B = sympify(b, False)
        if A is None or B is None: return NotImplemented
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

    def inv(self):
        result = DenseMatrix(self.nrows(), self.ncols(), [0]*self.nrows()*self.ncols())
        deref(self.thisptr).inv(deref(result.thisptr))
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

    def LU_solve(self, b):
        cdef DenseMatrix b_ = sympify(b)
        x = DenseMatrix(b_.nrows(), b_.ncols(), [0]*b_.nrows()*b_.ncols())
        deref(self.thisptr).LU_solve(deref(b_.thisptr), deref(x.thisptr))
        return x

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

def sin(x):
    cdef Basic X = sympify(x)
    return c2py(csympy.sin(X.thisptr))

def cos(x):
    cdef Basic X = sympify(x)
    return c2py(csympy.cos(X.thisptr))

def function_symbol(name, x):
    cdef Basic X = sympify(x)
    return c2py(csympy.function_symbol(name.encode("utf-8"), X.thisptr))

def sqrt(x):
    cdef Basic X = sympify(x)
    return c2py(csympy.sqrt(X.thisptr))

def densematrix(row, col, l):
    return DenseMatrix(row, col, l)

I = c2py(csympy.I)

# Turn on nice stacktraces:
csympy.print_stack_on_segfault()
