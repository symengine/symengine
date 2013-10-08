from cython.operator cimport dereference as deref
cimport csympy
from csympy cimport rcp, RCP
from libcpp.string cimport string

class SympifyError(Exception):
    pass

cdef c2py(RCP[csympy.Basic] o):
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
    elif (csympy.is_a_Symbol(deref(o))):
        # TODO: figure out how to bypass the __init__() completely:
        r = Symbol.__new__(Symbol, "null")
    elif (csympy.is_a_Sin(deref(o))):
        r = Sin.__new__(Sin)
    elif (csympy.is_a_Cos(deref(o))):
        r = Cos.__new__(Cos)
    elif (csympy.is_a_FunctionSymbol(deref(o))):
        r = FunctionSymbol.__new__(FunctionSymbol)
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
    elif isinstance(a, sympy.sin):
        return sin(a.args[0])
    elif isinstance(a, sympy.cos):
        return cos(a.args[0])
    elif isinstance(a, sympy.Function):
        name = str(a.func)
        arg = a.args[0]
        return function_symbol(name, sympy2csympy(arg, True))
    if raise_error:
        raise SympifyError("sympy2csympy: Cannot convert '%r' to a csympy type." % a)

def sympify(a, raise_error=True):
    if isinstance(a, Basic):
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
    cdef RCP[csympy.Basic] thisptr

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
        cdef RCP[csympy.Symbol] X = csympy.rcp_static_cast_Symbol(x.thisptr)
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


cdef class Symbol(Basic):

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Symbol(name.encode("utf-8")))

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.Symbol] X = csympy.rcp_static_cast_Symbol(self.thisptr)
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
            tmp = str(i)
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

cdef class Add(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.Add] X = csympy.rcp_static_cast_Add(self.thisptr)
        cdef RCP[csympy.Basic] a, b
        deref(X).as_two_terms(csympy.outArg(a), csympy.outArg(b))
        return c2py(a)._sympy_() + c2py(b)._sympy_()

cdef class Mul(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.Mul] X = csympy.rcp_static_cast_Mul(self.thisptr)
        cdef RCP[csympy.Basic] a, b
        deref(X).as_two_terms(csympy.outArg(a), csympy.outArg(b))
        return c2py(a)._sympy_() * c2py(b)._sympy_()

cdef class Pow(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.Pow] X = csympy.rcp_static_cast_Pow(self.thisptr)
        cdef RCP[csympy.Basic] base, exp
        base = deref(X).base_
        exp = deref(X).exp_
        return c2py(base)._sympy_() ** c2py(exp)._sympy_()

cdef class Function(Basic):
    pass

cdef class Sin(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.Sin] X = csympy.rcp_static_cast_Sin(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.sin(arg)

cdef class Cos(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.Cos] X = csympy.rcp_static_cast_Cos(self.thisptr)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.cos(arg)

cdef class FunctionSymbol(Function):

    def __dealloc__(self):
        self.thisptr.reset()

    def _sympy_(self):
        cdef RCP[csympy.FunctionSymbol] X = \
            csympy.rcp_static_cast_FunctionSymbol(self.thisptr)
        name = deref(X).get_name().decode("utf-8")
        # In Python 2.7, function names cannot be unicode:
        name = str(name)
        arg = c2py(deref(X).get_arg())._sympy_()
        import sympy
        return sympy.Function(name)(arg)

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

# Turn on nice stacktraces:
csympy.print_stack_on_segfault()
