# -*- coding: utf-8 -*-

import os

import sympy as sm


def _load_matrix():

    path = os.path.join(os.path.split(__file__)[0], 'eom.txt')

    with open(path, 'r') as f:
        txt = f.read()

    return sm.sympify(txt)


class TimeJacobian:

    def setup(self):

        self.matrix = _load_matrix()

        t = sm.symbols('t')

        self.states = [s(t) for s in sm.symbols('q0, q1, q2, q3, q4, q5, '
                                                'u0, u1, u2, u3, u4, u5',
                                                cls=sm.Function)]

        self.subs2 = sm.symbols('s:{}'.format(len(self.states)))

        self.subs = dict(zip(self.states, self.subs2))
        self.matrix = self.matrix.subs(self.subs)

    def time_jacobian(self):

        self.J = self.matrix.jacobian(self.subs2)
