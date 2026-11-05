#include "addition.hpp"
#include <cassert>
#include <stdexcept>
int main() {
    numlib::set_random_seed(12345);
    auto first = numlib::random_uniform(8, -1.0, 1.0);
    numlib::set_random_seed(12345);
    auto second = numlib::random_uniform(8, -1.0, 1.0);
    assert(first == second);
    for (double x : first) assert(x >= -1.0 && x < 1.0);
    auto ints = numlib::random_integer(100, 2, 5);
    for (int x : ints) assert(x >= 2 && x <= 5);
    bool caught = false;
    try { (void)numlib::random_uniform(2, 5.0, 1.0); }
    catch (const std::invalid_argument&) { caught = true; }
    assert(caught);
}
