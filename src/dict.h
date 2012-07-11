#ifndef CSYMPY_DICT_H
#define CSYMPY_DICT_H

#include <cstddef>
#include "Teuchos_RCP.hpp"

namespace CSymPy {

// This implements a "dictionary" of CSymPy objects:

// Our hash:
typedef struct
{
    long operator() (const Teuchos::RCP<Basic> &k) const {
        return k->__hash__();
    }
} RCPBasicHash;

// Our comparison:
typedef struct
{
    bool operator() (const Teuchos::RCP<Basic> &x, const Teuchos::RCP<Basic> &y) const {
        return x->__eq__(*y);
    }
} RCPBasicKeyEq;


typedef std::unordered_map<Teuchos::RCP<Basic>, int, RCPBasicHash,
        RCPBasicKeyEq> Dict;

} // CSymPy


#endif
