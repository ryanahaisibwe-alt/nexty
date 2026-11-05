#ifndef SHADIC_BROADCASTING_HPP
#define SHADIC_BROADCASTING_HPP

#include "ndarray.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace shadic {
class ShapeTracker {
public:
    using Shape = std::vector<std::size_t>;

    // Dimensions are aligned from the right, like NumPy.
    // Two dimensions are compatible when they are equal or either is 1.
    static Shape broadcast_shapes(const Shape& shape1, const Shape& shape2);
};

// These perform element-wise operations with broadcasting, returning the
// computed broadcast shape. They throw invalid_argument for incompatible shapes.
NDArray broadcast_add(const NDArray& a, const NDArray& b);
NDArray broadcast_subtract(const NDArray& a, const NDArray& b);
NDArray broadcast_multiply(const NDArray& a, const NDArray& b);
NDArray broadcast_divide(const NDArray& a, const NDArray& b);
} // namespace shadic

#endif
