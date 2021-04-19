---
jupytext:
  formats: ipynb,md:myst
  text_representation:
    extension: .md
    format_name: myst
    format_version: 0.13
    jupytext_version: 1.10.0
kernelspec:
  display_name: C++11
  language: C++11
  name: xcpp11
---

# Matrices

```{note}
As of now there is no pretty printer implemented for matrices ([issue](https://github.com/symengine/symengine/issues/1701))
```


The following header files will be necessary for this section.

```{code-cell}
#include <symengine/parser.h>
#include <symengine/matrix.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/symengine_exception.h>
#include <symengine/visitor.h>
#include <symengine/printers/strprinter.h>
```

## Base Elements

We will need a set of basic elements to cover a reasonable set of operations. Namely we would like:

- Two Matrices (A,B)
- A Vector (X)

```{code-cell}
SymEngine::vec_basic elemsA{SymEngine::integer(1),
                           SymEngine::integer(0),
                           SymEngine::integer(-1),
                           SymEngine::integer(-2)};
SymEngine::DenseMatrix A = SymEngine::DenseMatrix(2, 2, elemsA);
```

```{code-cell}
A.__str__()
```

```{code-cell}
SymEngine::vec_basic elemsB{SymEngine::integer(5),
                           SymEngine::integer(2),
                           SymEngine::integer(-7),
                           SymEngine::integer(-3)};
SymEngine::DenseMatrix B = SymEngine::DenseMatrix(2, 2, elemsB);
B.__str__()
```

Note that the real utility of working with SymEngine is the ability to include free symbols as first class members of the matrix.

+++

## Basic Operations

The key thing to remember that as a `C++` library, we need to pre-allocated variable sizes and types. Furthermore, the unary operators are **not** overloaded, so we will call functions for each of the standard operations. The general form of each of these is:

**operation**(_term1_,_term2_,**output**)

+++

### Matrix-Matrix

#### Addition
The addition of two dense matrices is carried out as expected.

\begin{align*}
\mathbf{A}+\mathbf{B} & = \begin{bmatrix}
 a_{11} & a_{12} & \cdots & a_{1n} \\
 a_{21} & a_{22} & \cdots & a_{2n} \\
 \vdots & \vdots & \ddots & \vdots \\
 a_{m1} & a_{m2} & \cdots & a_{mn} \\
\end{bmatrix} +

\begin{bmatrix}
 b_{11} & b_{12} & \cdots & b_{1n} \\
 b_{21} & b_{22} & \cdots & b_{2n} \\
 \vdots & \vdots & \ddots & \vdots \\
 b_{m1} & b_{m2} & \cdots & b_{mn} \\
\end{bmatrix} \\
& = \begin{bmatrix}
 a_{11} + b_{11} & a_{12} + b_{12} & \cdots & a_{1n} + b_{1n} \\
 a_{21} + b_{21} & a_{22} + b_{22} & \cdots & a_{2n} + b_{2n} \\
 \vdots & \vdots & \ddots & \vdots \\
 a_{m1} + b_{m1} & a_{m2} + b_{m2} & \cdots & a_{mn} + b_{mn} \\
\end{bmatrix} \\
\end{align*}

```{code-cell}
SymEngine::DenseMatrix C = SymEngine::DenseMatrix(2, 2);
add_dense_dense(A, B, C);
C.__str__()
```

#### Multiplication

+++

### Matrix-Scalar
#### Addition

## Gaussian Elimination

One of the main use-cases for any library with matrices is the ability to perform Gaussian elimination. We will consider an example from the literature {cite}`nakosFractionfreeAlgorithmsLinear1997`.

```{code-cell}
    // Fraction-Free Algorithms for Linear and Polynomial Equations, George C
    // Nakos,
    // Peter R Turner et. al.
   SymEngine::DenseMatrix A = SymEngine::DenseMatrix(4, 4, {SymEngine::integer(1), SymEngine::integer(2), SymEngine::integer(3), SymEngine::integer(4),
                           SymEngine::integer(2), SymEngine::integer(2), SymEngine::integer(3), SymEngine::integer(4),
                           SymEngine::integer(3), SymEngine::integer(3), SymEngine::integer(3), SymEngine::integer(4),
                           SymEngine::integer(9), SymEngine::integer(8), SymEngine::integer(7), SymEngine::integer(6)});
  SymEngine::DenseMatrix  B = SymEngine::DenseMatrix(4, 4);
fraction_free_gaussian_elimination(A, B);
B.__str__()
```

## Hadamard Product

```{warning}
This will be part of the next release
```


This is essentially the element wise multiplication of two matrices, so for two matrices of the same dimension we have $(A \circ B)_{ij} = (A \odot B)_{ij} = (A)_{ij} (B)_{ij}$.

$$
\begin{bmatrix}
    a_{11} & a_{12} & a_{13}\\
    a_{21} & a_{22} & a_{23}\\
    a_{31} & a_{32} & a_{33}
  \end{bmatrix} \circ \begin{bmatrix}
    b_{11} & b_{12} & b_{13}\\
    b_{21} & b_{22} & b_{23}\\
    b_{31} & b_{32} & b_{33}
  \end{bmatrix} = \begin{bmatrix}
    a_{11}\, b_{11} & a_{12}\, b_{12} & a_{13}\, b_{13}\\
    a_{21}\, b_{21} & a_{22}\, b_{22} & a_{23}\, b_{23}\\
    a_{31}\, b_{31} & a_{32}\, b_{32} & a_{33}\, b_{33}
\end{bmatrix}
$$

For `SymEngine` this has been implemented recently as the `element_mul_matrix` function.

```{code-cell}
SymEngine::DenseMatrix D = SymEngine::DenseMatrix(2, 2);
B.elementwise_mul_matrix(A, D);
D.__str__()
```

# NumPY Convenience Functions
A set of functions are provided to allow for `numpy`-esque generation of matrices.
- `eye` for generating a 2-D matrix with ones on the diagonal and zeros elsewhere
- `diag` for generating diagonal matrices
- `ones` for generating a dense matrix of ones
- `zeros` for generating a dense matrix of zeros

+++

## Identity Matrices and Offsets
The `eye` function is used to create 2D matrices with ones on the diagonal and zeros elsewhere; with an optional parameter to offset the position of the ones.

```{code-cell}
SymEngine::DenseMatrix npA = SymEngine::DenseMatrix(3, 3);
SymEngine::eye(npA);
npA.__str__()
```

```{code-cell}
SymEngine::eye(npA,1);
npA.__str__()
```

## Diagonal Matrix Helpers
The `diag` function

```{bibliography} references.bib
```
