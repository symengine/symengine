from sympy.printing.pycode import PythonCodePrinter


class SymEnginePrinter(PythonCodePrinter):

    def _print_Pow(self, expr):
        return "pow({0}, {1})".format(self._print(expr.base), self._print(expr.exp))

    def _print_Add(self, expr):
        if len(expr.args) != 2:
            raise NotImplementedError
        return "add({}, {})".format(
            self._print(expr.args[0]),
            self._print(expr.args[1]),
        )


def symengine_print(expr):
    printer = SymEnginePrinter()
    return printer.doprint(expr)
