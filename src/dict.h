#ifndef CSYMPY_DICT_H
#define CSYMPY_DICT_H

#include "basic.h"
#include "integer.h"

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


typedef std::unordered_map<Teuchos::RCP<Basic>, Teuchos::RCP<Integer>,
        RCPBasicHash, RCPBasicKeyEq> Dict_int;

// Return true if the two dictionaries 'a' and 'b' are equal. Otherwise false.
bool dicts_equal(const Dict_int &a, const Dict_int &b);

} // CSymPy

std::ostream& operator<<(std::ostream& out, const CSymPy::Dict_int& d);

#endif
