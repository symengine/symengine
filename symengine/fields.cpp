#include <symengine/fields.h>

namespace SymEngine
{

GaloisField::GaloisField(std::map<unsigned, int> &dict, unsigned modulo) : modulo_(modulo)
{
	unsigned temp;
	for (auto iter : dict) {
		temp = iter.second % modulo;
		if (temp != 0)
			dict_[iter.first] = temp;
	}
	// if (dict_.empty()) {
	// 	dict_[0] = 0;
	// }
}

GaloisField::GaloisField(std::vector<int> &dict, unsigned modulo) : modulo_(modulo)
{
	unsigned temp;
	for (unsigned i = 0; i < dict.size(); i++) {
		temp = dict[i] % modulo;
		if (temp != 0)
			dict_[i] = temp;	
	}
}

std::map<unsigned, int> GaloisField::gf_neg()
{
	return gf_mul_ground(-1);
}

std::map<unsigned, int> GaloisField::gf_add_ground(int a)
{
	auto dict = dict_;
	dict[0] = (dict[0] + a) % modulo_;
	return std::move(dict);
}

std::map<unsigned, int> GaloisField::gf_sub_ground(int a)
{
	return gf_add_ground(-1 * a);
}

std::map<unsigned, int> GaloisField::gf_mul_ground(int a)
{
	std::map<unsigned, int> dict;
	for (auto iter : dict_) {
		dict[iter.first] = (a * dict[iter.second]) % modulo_;
	}
	return std::move(dict);
}
// TODO invert and gf_quo_ground and gcdex
std::map<unsigned, int> GaloisField::gf_add(std::map<unsigned, int> &dict_in)
{
	auto dict_out = dict_;
	for (auto iter : dict_in) {
		dict_out[iter.first] = (dict_out[iter.first] + iter.second) % modulo_;
	}
	return std::move(dict_out);
}

std::map<unsigned, int> GaloisField::gf_sub(std::map<unsigned, int> &dict_in)
{
	auto dict_out = dict_;
	for (auto iter : dict_in) {
		dict_out[iter.first] = (dict_out[iter.first] - iter.second) % modulo_;
	}
	return std::move(dict_out);
}

std::map<unsigned, int> GaloisField::gf_mul(std::map<unsigned, int> &dict_in)
{
	std::map<unsigned, int> dict_out;
	for (auto iter : dict_in) {
		for (auto it : dict_) {
			dict_out[iter.first * it.first] = dict_out[iter.first * it.second]
												+ iter.second * it.second;
		}
	}
	return std::move(dict_out);
}
}