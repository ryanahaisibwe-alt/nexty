#include "indexing.hpp"
#include <stdexcept>
namespace indexing {
double get(const NDArray& a,const std::vector<std::size_t>& i){return a.at(i);}
void set(NDArray& a,const std::vector<std::size_t>& i,double v){a.at(i)=v;}
std::vector<std::size_t> shape(const NDArray& a){return a.shape();}
std::size_t dimensions(const NDArray& a){return a.ndim();}
std::size_t size(const NDArray& a){return a.size();}
static std::vector<std::size_t> coords(std::size_t n,const std::vector<std::size_t>& s){
 std::vector<std::size_t>x(s.size());for(std::size_t i=s.size();i>0;--i){x[i-1]=n%s[i-1];n/=s[i-1];}return x;
}
NDArray slice(const NDArray& a,std::size_t axis,std::size_t start,std::size_t stop,std::size_t step){
 auto s=a.shape();if(axis>=s.size()||step==0||start>stop||stop>s[axis])throw std::invalid_argument("Invalid slice boundaries or axis");
 s[axis]=stop<=start?0:(stop-start+step-1)/step;std::size_t n=1;for(auto d:s)n*=d;
 std::vector<double> out;out.reserve(n);for(std::size_t i=0;i<n;++i){auto ix=coords(i,s);ix[axis]=start+ix[axis]*step;out.push_back(a.at(ix));}
 return NDArray(s,out);
}
NDArray reshape(const NDArray& a,const std::vector<std::size_t>& s){return NDArray(s,a.values());}
}
