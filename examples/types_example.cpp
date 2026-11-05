#include "array_utils.hpp"
#include <iostream>
int main(){std::vector<int> raw={1,2,3};auto cv=array_utils::convert<double>(raw);std::cout<<"Converted: ";for(auto x:cv)std::cout<<x<<" ";
 NDArray a({3},{3,1,2}),b({3},{10,20,30});auto s=array_utils::sort(a);std::cout<<"\nSorted: ";for(auto x:s.values())std::cout<<x;
 std::cout<<"\nDot product: "<<array_utils::dot(a,b)<<"\nFirst index of 2: "<<array_utils::find(a,2);
 array_utils::save(a,"array_data.txt");auto loaded=array_utils::load("array_data.txt");std::cout<<"\nLoaded: ";for(auto x:loaded.values())std::cout<<x<<" ";std::cout<<"\n";}
