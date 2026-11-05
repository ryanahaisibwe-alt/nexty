#include "array_creation.hpp"
#include <iostream>
int main(){auto z=array_creation::zeros({2,2});auto o=array_creation::ones({2,2});auto r=array_creation::arange(0,5);
 std::cout<<"Zeros: ";for(auto x:z.values())std::cout<<x<<" ";std::cout<<"\nOnes: ";for(auto x:o.values())std::cout<<x<<" ";
 std::cout<<"\nRange: ";for(auto x:r.values())std::cout<<x<<" ";NDArray a({2,2},{1,2,3,4}),b({2,2},{5,6,7,8});
 auto joined=array_creation::concatenate({a,b},0);std::cout<<"\nJoined: ";for(auto x:joined.values())std::cout<<x<<" ";
 auto pieces=array_creation::split(joined,0,2);std::cout<<"\nFirst split: ";for(auto x:pieces[0].values())std::cout<<x<<" ";std::cout<<"\n";}
