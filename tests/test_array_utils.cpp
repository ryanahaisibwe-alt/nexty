#include "array_utils.hpp"
#include <cassert>
#include <cstdio>
#include <vector>
int main(){std::vector<int>x={1,2};assert((array_utils::convert<double>(x)==std::vector<double>{1,2}));
 NDArray a({3},{3,1,2}),b({3},{10,20,30});assert((array_utils::sort(a).values()==std::vector<double>{1,2,3}));
 assert((array_utils::search(a,2)==std::vector<std::size_t>{2}));assert(array_utils::find(a,99)==-1);
 assert((array_utils::add(a,b).values()==std::vector<double>{13,21,32}));assert((array_utils::subtract(b,a).values()==std::vector<double>{7,19,28}));
 assert((array_utils::multiply(a,b).values()==std::vector<double>{30,20,60}));assert(array_utils::dot(a,b)==110);
 array_utils::save(a,"array_test.txt");auto loaded=array_utils::load("array_test.txt");assert(loaded.values()==a.values());std::remove("array_test.txt");}
