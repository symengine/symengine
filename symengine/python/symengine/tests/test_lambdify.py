# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import symengine as se
import numpy as np
from symengine import Lambdify

def test_Lambdify():
    pass
    n = 7
    args = x, y, z = se.symbols('x y z')
    l = Lambdify(args, [x+y+z, x**2, (x-y)/z, x*y*z])
    assert np.allclose(l(range(n, n+len(args))),
                       [3*n+3, n**2, -1/(n+2), n*(n+1)*(n+2)])
