/**
 *  \file fields.h
 *
 **/
#ifndef SYMENGINE_FIELDS_H
#define SYMENGINE_FIELDS_H

#include <symengine/basic.h>

namespace SymEngine
{
class GaloisField : public Basic
{
public:
	std::map<unsigned, int> dict_;
	unsigned modulo_;

public:
	IMPLEMENT_TYPEID(GALOISFIELD)
	GaloisField(std::map<unsigned, int> &dict, unsigned modulo);
	GaloisField(std::vector<int> &dict, unsigned modulo);

	std::map<unsigned, int> gf_neg();
	std::map<unsigned, int> gf_add_ground(int a);
	std::map<unsigned, int> gf_sub_ground(int a);
	std::map<unsigned, int> gf_mul_ground(int a);

	std::map<unsigned, int> gf_add(std::map<unsigned, int> &dict);
	std::map<unsigned, int> gf_sub(std::map<unsigned, int> &dict);
	std::map<unsigned, int> gf_mul(std::map<unsigned, int> &dict);

};
}

#endif
