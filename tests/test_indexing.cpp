#include "indexing.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>
int main(){NDArray a({2,3},{1,2,3,4,5,6});assert(indexing::get(a,{1,2})==6);
 indexing::set(a,{0,0},9);assert(indexing::get(a,{0,0})==9);auto s=indexing::slice(a,1,1,3);
 assert((s.shape()==std::vector<std::size_t>{2,2}));assert((s.values()==std::vector<double>{2,3,5,6}));
 auto r=indexing::reshape(a,{3,2});assert((r.shape()==std::vector<std::size_t>{3,2}));
 bool caught=false;try{indexing::reshape(a,{4,2});}catch(const std::invalid_argument&){caught=true;}assert(caught);}
