#ifndef AGGREGATION_HPP
#define AGGREGATION_HPP

#include "ndarray.hpp"
#include <cstddef>

namespace aggregation {

    // Sum all elements in an array
    double sum(const NDArray& array);

    // Reduce an array by summing along an axis
    NDArray sum(const NDArray& array, std::size_t axis);

    // Find minimum and maximum along an axis
    NDArray minimum(const NDArray& array, std::size_t axis);
    NDArray maximum(const NDArray& array, std::size_t axis);

    // Add two arrays element by element
    NDArray add(const NDArray& first, const NDArray& second);

}

#endif
