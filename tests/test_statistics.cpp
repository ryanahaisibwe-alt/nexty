#include "statistics.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>
int main() {
    const std::vector<double> x{1,2,3,4,5};
    assert(statistics::mean(x) == 3.0);
    assert(statistics::median(x) == 3.0);
    assert(statistics::minimum(x) == 1.0);
    assert(statistics::maximum(x) == 5.0);
    assert(statistics::count(x) == 5);
    assert(std::fabs(statistics::standard_deviation(x)-std::sqrt(2.0)) < 1e-12);
    assert(statistics::quantile(x,0.25) == 2.0);
    assert(statistics::percentile(x,75) == 4.0);
    assert(std::fabs(statistics::correlation(x,{2,4,6,8,10})-1.0)<1e-12);
    bool caught=false;
    try { statistics::correlation({1,1,1},{2,3,4}); }
    catch(const std::domain_error&) { caught=true; }
    assert(caught);
    return 0;
}
