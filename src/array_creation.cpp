#include "array_creation.hpp"
#include <stdexcept>
namespace array_creation {
static std::size_t count(const std::vector<std::size_t>& s){if(s.empty())throw std::invalid_argument("Empty shape");std::size_t n=1;for(auto d:s){if(!d)throw std::invalid_argument("Zero dimension");n*=d;}return n;}
NDArray zeros(const std::vector<std::size_t>& s){return NDArray(s,std::vector<double>(count(s),0));}
NDArray ones(const std::vector<std::size_t>& s){return NDArray(s,std::vector<double>(count(s),1));}
NDArray full(const std::vector<std::size_t>& s,double v){return NDArray(s,std::vector<double>(count(s),v));}
NDArray arange(double a,double b,double step){if(step==0||(b>a&&step<0)||(b<a&&step>0))throw std::invalid_argument("Invalid range step");std::vector<double>v;for(double x=a;(step>0?x<b:x>b);x+=step)v.push_back(x);if(v.empty())throw std::invalid_argument("Empty range");return NDArray({v.size()},v);}
static std::vector<std::size_t> coords(std::size_t n,const std::vector<std::size_t>&s){std::vector<std::size_t>x(s.size());for(std::size_t i=s.size();i>0;--i){x[i-1]=n%s[i-1];n/=s[i-1];}return x;}
NDArray concatenate(const std::vector<NDArray>& a,std::size_t axis){if(a.empty())throw std::invalid_argument("No arrays");auto s=a[0].shape();if(axis>=s.size())throw std::invalid_argument("Bad axis");std::size_t joined=0;
 for(const auto&x:a){auto t=x.shape();if(t.size()!=s.size())throw std::invalid_argument("Rank mismatch");for(std::size_t d=0;d<s.size();++d)if(d!=axis&&t[d]!=s[d])throw std::invalid_argument("Shape mismatch");joined+=t[axis];}
 s[axis]=joined;std::size_t n=count(s);std::vector<double>out;out.reserve(n);for(std::size_t i=0;i<n;++i){auto ix=coords(i,s);std::size_t p=ix[axis];const NDArray*chosen=nullptr;for(const auto&x:a){auto t=x.shape();if(p<t[axis]){chosen=&x;break;}p-=t[axis];}ix[axis]=p;out.push_back(chosen->at(ix));}return NDArray(s,out);}
std::vector<NDArray> split(const NDArray&a,std::size_t axis,std::size_t sections){auto s=a.shape();if(axis>=s.size()||sections==0||s[axis]%sections)throw std::invalid_argument("Invalid split axis/sections");std::size_t w=s[axis]/sections;std::vector<NDArray>out;
 for(std::size_t p=0;p<sections;++p){auto t=s;t[axis]=w;std::size_t n=count(t);std::vector<double>v;v.reserve(n);for(std::size_t i=0;i<n;++i){auto ix=coords(i,t);ix[axis]+=p*w;v.push_back(a.at(ix));}out.emplace_back(t,v);}return out;}
}
