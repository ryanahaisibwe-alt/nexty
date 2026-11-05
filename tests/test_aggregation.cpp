#include "aggregation.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>
int main() {
    NDArray a({2,3},{1,2,3,4,5,6});
    assert(aggregation::sum(a)==21.0);
    assert((aggregation::sum(a,0).values()==std::vector<double>{5,7,9}));
    assert((aggregation::sum(a,1).values()==std::vector<double>{6,15}));
    assert((aggregation::minimum(a,1).values()==std::vector<double>{1,4}));
    assert((aggregation::maximum(a,1).values()==std::vector<double>{3,6}));
    bool caught=false;
    try { aggregation::sum(a,2); } catch(const std::invalid_argument&) { caught=true; }
    assert(caught);
    return 0;
}
