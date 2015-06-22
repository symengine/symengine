set(PY_PATH ${PYTHON_INSTALL_PATH}/symengine/tests)
install(FILES __init__.py
    test_arit.py
    test_eval.py
    test_functions.py
    test_integer.py
    test_matrices.py
    test_ntheory.py
    test_subs.py
    test_symbol.py
    test_sympify.py
    test_sympy_conv.py
    test_var.py
    DESTINATION ${PY_PATH}
    )
