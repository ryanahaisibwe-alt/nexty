#include "indexing.hpp"
#include <iostream>
int main(){NDArray a({2,3},{1,2,3,4,5,6});std::cout<<indexing::get(a,{1,2})<<"\n";
 indexing::set(a,{0,1},20);auto part=indexing::slice(a,1,0,2);for(double x:part.values())std::cout<<x<<" ";
 auto reshaped=indexing::reshape(a,{3,2});std::cout<<"\nShape: ";for(auto d:reshaped.shape())std::cout<<d<<" ";std::cout<<"\n";}
