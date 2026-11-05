#include "broadcasting.hpp"
#include <cassert>
#include <stdexcept>
int main() {
    using shadic::ShapeTracker;
    assert((ShapeTracker::broadcast_shapes({2,1,4}, {3,4}) == ShapeTracker::Shape{2,3,4}));
    assert((ShapeTracker::broadcast_shapes({2,3}, {3}) == ShapeTracker::Shape{2,3}));
    NDArray a({2,1}, {10,20});
    NDArray b({1,3}, {1,2,3});
    auto sum = shadic::broadcast_add(a,b);
    assert((sum.shape() == std::vector<std::size_t>{2,3}));
    assert((sum.values() == std::vector<double>{11,12,13,21,22,23}));
    auto product = shadic::broadcast_multiply(a,b);
    assert((product.values() == std::vector<double>{10,20,30,20,40,60}));
    bool caught=false;
    try { (void)shadic::broadcast_add(NDArray({2,3},{1,2,3,4,5,6}), NDArray({2,2},{1,2,3,4})); }
    catch (const std::invalid_argument&) { caught=true; }
    assert(caught);
}
