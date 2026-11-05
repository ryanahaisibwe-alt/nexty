#pragma once
#include "ndarray.hpp"
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
namespace array_utils {
template<class To,class From> std::vector<To> convert(const std::vector<From>& in){
 static_assert(std::is_arithmetic<To>::value&&std::is_arithmetic<From>::value,"Numeric types only");
 std::vector<To> out;out.reserve(in.size());for(auto x:in)out.push_back(static_cast<To>(x));return out;}
NDArray sort(const NDArray&,bool ascending=true);
std::vector<std::size_t> search(const NDArray&,double target);
long long find(const NDArray&,double target);
NDArray add(const NDArray&,const NDArray&); NDArray subtract(const NDArray&,const NDArray&);
NDArray multiply(const NDArray&,const NDArray&); NDArray divide(const NDArray&,const NDArray&);
double dot(const NDArray&,const NDArray&);
void save(const NDArray&,const std::string& filename); NDArray load(const std::string& filename);
}
