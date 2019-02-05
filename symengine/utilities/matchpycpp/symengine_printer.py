from sympy.printing.pycode import PythonCodePrinter
from sympy.printing import StrPrinter
from sympy import Add


class SymEnginePrinter(StrPrinter):

    def _print_Pow(self, expr):
        return "pow({0}, {1})".format(self._print(expr.base), self._print(expr.exp))

    def _print_Add(self, expr):
        if len(expr.args) != 2:
            return "add({}, {})".format(expr.args[0], self._print(Add.fromiter(expr.args[1:])))
        return "add({}, {})".format(
            self._print(expr.args[0]),
            self._print(expr.args[1]),
        )

    def _print_Integer(self, expr):
        return "integer({})".format(expr)

    def _print_int(self, expr):
        return self._print_Integer(expr)


def symengine_print(expr):
    printer = SymEnginePrinter()
    return printer.doprint(expr)
