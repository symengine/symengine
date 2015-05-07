/**
 * \file diophantine.h
 * Algorithms for Diophantine equations
 *
 **/

#ifndef CSYMPY_DIOPHANTINE_H
#define CSYMPY_DIOPHANTINE_H

#include <vector>

#include "matrix.h"

namespace SymEngine {

// Solve the diophantine system Ax = 0 and return a basis set for solutions
void homogeneous_lde(std::vector<DenseMatrix> &basis, const DenseMatrix &A);

}

#endif
