#include "array_utils.hpp"
#include <functional>
#include <sstream>
namespace array_utils {
NDArray sort(const NDArray&a,bool asc){auto v=a.values();if(asc)std::sort(v.begin(),v.end());else std::sort(v.begin(),v.end(),std::greater<double>());return NDArray(a.shape(),v);}
std::vector<std::size_t> search(const NDArray&a,double t){std::vector<std::size_t>r;for(std::size_t i=0;i<a.size();++i)if(a.values()[i]==t)r.push_back(i);return r;}
long long find(const NDArray&a,double t){for(std::size_t i=0;i<a.size();++i)if(a.values()[i]==t)return static_cast<long long>(i);return -1;}
static void same(const NDArray&a,const NDArray&b){if(a.shape()!=b.shape())throw std::invalid_argument("Shapes must match");}
NDArray add(const NDArray&a,const NDArray&b){same(a,b);std::vector<double>v(a.size());for(std::size_t i=0;i<v.size();++i)v[i]=a.values()[i]+b.values()[i];return NDArray(a.shape(),v);}
NDArray subtract(const NDArray&a,const NDArray&b){same(a,b);std::vector<double>v(a.size());for(std::size_t i=0;i<v.size();++i)v[i]=a.values()[i]-b.values()[i];return NDArray(a.shape(),v);}
NDArray multiply(const NDArray&a,const NDArray&b){same(a,b);std::vector<double>v(a.size());for(std::size_t i=0;i<v.size();++i)v[i]=a.values()[i]*b.values()[i];return NDArray(a.shape(),v);}
NDArray divide(const NDArray&a,const NDArray&b){same(a,b);std::vector<double>v(a.size());for(std::size_t i=0;i<v.size();++i){if(b.values()[i]==0)throw std::domain_error("Division by zero");v[i]=a.values()[i]/b.values()[i];}return NDArray(a.shape(),v);}
double dot(const NDArray&a,const NDArray&b){if(a.ndim()!=1||b.ndim()!=1||a.size()!=b.size())throw std::invalid_argument("Dot product needs equal-length 1D arrays");double r=0;for(std::size_t i=0;i<a.size();++i)r+=a.values()[i]*b.values()[i];return r;}
void save(const NDArray&a,const std::string&name){std::ofstream f(name);if(!f)throw std::runtime_error("Cannot open output file");auto s=a.shape();for(std::size_t i=0;i<s.size();++i){if(i)f<<',';f<<s[i];}f<<'\n';for(std::size_t i=0;i<a.size();++i){if(i)f<<',';f<<a.values()[i];}f<<'\n';if(!f)throw std::runtime_error("Write failed");}
NDArray load(const std::string&name){std::ifstream f(name);if(!f)throw std::runtime_error("Cannot open input file");std::string line,item;if(!std::getline(f,line))throw std::runtime_error("Missing shape");std::vector<std::size_t>s;std::stringstream ss(line);while(std::getline(ss,item,','))s.push_back(std::stoull(item));if(!std::getline(f,line))throw std::runtime_error("Missing data");std::vector<double>v;std::stringstream ds(line);while(std::getline(ds,item,','))v.push_back(std::stod(item));return NDArray(s,v);}
}
