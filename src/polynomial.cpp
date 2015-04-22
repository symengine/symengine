#include "polynomial.h"
#include "add.h"

namespace CSymPy {

    Polynomial::Polynomial(const std::string& var, map_uint_integer&& dict) :
         var_{var}, dict_{std::move(dict)} {

        map_uint_integer::iterator it = dict_.begin();
        uint largest = it->first;
        uint cur;
        while(it!=dict_.end()){
            cur =it->first;
            if(cur > largest)
                largest = cur;
            it++;
        }
        degree = largest;
    }

    RCP<const Polynomial> add_poly(const Polynomial &a, const Polynomial &b) {
        map_uint_integer dict;
        for(auto &it : a.dict_)
            dict[it.first] = integer(0);
        for(auto &it : b.dict_)
            dict[it.first] = integer(0);
        for(auto &it : a.dict_)
            dict[it.first] = rcp_static_cast<const Integer>(add(dict[it.first],(it.second)));
        for(auto &it : b.dict_)
            dict[it.first] = rcp_static_cast<const Integer>(add(dict[it.first], (it.second)));

        RCP<const Polynomial> c = polynomial(a.var_, std::move(dict));
        return c;
    }
}
