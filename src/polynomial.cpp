#include "polynomial.h"

namespace CSymPy {

    //polynomial::polynomial (const RCP<const Basic> &var, umap_basic_num&& dict) :
   //degree{-1}, var_{var}, dict_{std::move(dict)}{}
   // void polynomial::add(const RCP<const poynomial> &b, const RCP<const polynomial> &c){

    Polynomial::Polynomial(const std::string& var, map_integer_uint&& dict) :
         var_{var}, dict_{std::move(dict)} {

        map_integer_uint::iterator it = dict_.begin();
        int largest = (it->first)->as_int();
        int cur;
        while(it!=dict_.end()){
            cur =(it->first)->as_int();
            if(cur > largest)
                largest = cur;
            it++;
        }
        degree = largest;
    }

}
