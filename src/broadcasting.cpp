#include "broadcasting.hpp"
#include <limits>
#include <string>

namespace shadic {
ShapeTracker::Shape ShapeTracker::broadcast_shapes(const Shape& a, const Shape& b) {
    const std::size_t rank = a.size() > b.size() ? a.size() : b.size();
    Shape result(rank, 1);
    for (std::size_t offset = 0; offset < rank; ++offset) {
        const std::size_t da = offset < a.size() ? a[a.size() - 1 - offset] : 1;
        const std::size_t db = offset < b.size() ? b[b.size() - 1 - offset] : 1;
        if (da != db && da != 1 && db != 1) {
            throw std::invalid_argument("Cannot broadcast shapes: dimensions must match or one dimension must be 1.");
        }
        result[rank - 1 - offset] = (da == db) ? da : (da == 1 ? db : da);
    }
    return result;
}

namespace {
std::vector<std::size_t> coordinates(std::size_t flat, const ShapeTracker::Shape& shape) {
    std::vector<std::size_t> index(shape.size(), 0);
    for (std::size_t i = shape.size(); i > 0; --i) {
        if (shape[i - 1] == 0) throw std::invalid_argument("Cannot index an array with a zero-sized dimension.");
        index[i - 1] = flat % shape[i - 1];
        flat /= shape[i - 1];
    }
    return index;
}
std::size_t flat_index(const std::vector<std::size_t>& index, const ShapeTracker::Shape& shape) {
    std::size_t flat = 0;
    for (std::size_t i = 0; i < shape.size(); ++i) flat = flat * shape[i] + index[i];
    return flat;
}
double read_broadcast_value(const NDArray& input, const std::vector<std::size_t>& output_index) {
    const auto in_shape = input.shape();
    const std::size_t offset = output_index.size() - in_shape.size();
    std::vector<std::size_t> in_index(in_shape.size());
    for (std::size_t i = 0; i < in_shape.size(); ++i)
        in_index[i] = in_shape[i] == 1 ? 0 : output_index[offset + i];
    return input.values()[flat_index(in_index, in_shape)];
}
template<class Op>
NDArray apply(const NDArray& a, const NDArray& b, Op op) {
    const auto shape = ShapeTracker::broadcast_shapes(a.shape(), b.shape());
    std::size_t count = 1;
    for (auto d : shape) {
        if (d != 0 && count > std::numeric_limits<std::size_t>::max() / d)
            throw std::overflow_error("Broadcast result is too large.");
        count *= d;
    }
    std::vector<double> values;
    values.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto idx = coordinates(i, shape);
        values.push_back(op(read_broadcast_value(a, idx), read_broadcast_value(b, idx)));
    }
    return NDArray(shape, values);
}
}
NDArray broadcast_add(const NDArray& a, const NDArray& b) {
    return apply(a, b, [](double x, double y) { return x + y; });
}
NDArray broadcast_subtract(const NDArray& a, const NDArray& b) {
    return apply(a, b, [](double x, double y) { return x - y; });
}
NDArray broadcast_multiply(const NDArray& a, const NDArray& b) {
    return apply(a, b, [](double x, double y) { return x * y; });
}
NDArray broadcast_divide(const NDArray& a, const NDArray& b) {
    return apply(a, b, [](double x, double y) {
        if (y == 0.0) throw std::domain_error("Division by zero during broadcasting.");
        return x / y;
    });
}
} // namespace shadic
