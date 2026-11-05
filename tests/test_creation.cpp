#include "array_creation.hpp"
#include <cassert>
#include <vector>
int main(){assert((array_creation::zeros({2}).values()==std::vector<double>{0,0}));assert((array_creation::ones({2}).values()==std::vector<double>{1,1}));
 assert((array_creation::full({2},4).values()==std::vector<double>{4,4}));assert((array_creation::arange(0,3).values()==std::vector<double>{0,1,2}));
 NDArray a({2,2},{1,2,3,4}),b({2,2},{5,6,7,8});auto c=array_creation::concatenate({a,b},0);assert((c.shape()==std::vector<std::size_t>{4,2}));
 auto p=array_creation::split(c,0,2);assert(p.size()==2);assert((p[1].values()==std::vector<double>{5,6,7,8}));}
