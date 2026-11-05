#include "aggregation.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

namespace aggregation {

    double sum(const NDArray& array)
    {
        double total = 0.0;

        for (double value : array.values()) {
            total += value;
        }

        return total;
    }

    // Convert a flat index into coordinates for the result shape.
    static std::vector<std::size_t> get_indices(
        std::size_t flat_index,
        const std::vector<std::size_t>& shape)
    {
        std::vector<std::size_t> indices(shape.size());

        for (std::size_t i = shape.size(); i > 0; --i) {
            indices[i - 1] = flat_index % shape[i - 1];
            flat_index /= shape[i - 1];
        }

        return indices;
    }

    // Shared implementation for sum, minimum and maximum reductions.
    static NDArray reduce(
        const NDArray& array,
        std::size_t axis,
        int operation)
    {
        const auto shape = array.shape();

        if (shape.size() < 2 || axis >= shape.size()) {
            throw std::invalid_argument(
                "Invalid axis: reduction requires at least 2 dimensions "
                "and a valid axis.");
        }

        std::vector<std::size_t> result_shape = shape;
        result_shape.erase(result_shape.begin() + axis);

        std::size_t result_size = 1;
        for (auto dimension : result_shape) {
            result_size *= dimension;
        }

        double initial = 0.0;

        if (operation == 1) {
            initial = std::numeric_limits<double>::infinity();
        } else if (operation == 2) {
            initial = -std::numeric_limits<double>::infinity();
        }

        std::vector<double> result(result_size, initial);

        for (std::size_t i = 0; i < result_size; ++i) {
            auto result_indices = get_indices(i, result_shape);
            std::vector<std::size_t> input_indices(shape.size());

            for (std::size_t j = 0, k = 0; j < shape.size(); ++j) {
                if (j != axis) {
                    input_indices[j] = result_indices[k++];
                }
            }

            for (std::size_t a = 0; a < shape[axis]; ++a) {
                input_indices[axis] = a;
                double value = array.at(input_indices);

                if (operation == 0) {
                    result[i] += value;
                } else if (operation == 1) {
                    result[i] = std::min(result[i], value);
                } else {
                    result[i] = std::max(result[i], value);
                }
            }
        }

        return NDArray(result_shape, result);
    }

    NDArray sum(const NDArray& array, std::size_t axis)
    {
        return reduce(array, axis, 0);
    }

    NDArray minimum(const NDArray& array, std::size_t axis)
    {
        return reduce(array, axis, 1);
    }

    NDArray maximum(const NDArray& array, std::size_t axis)
    {
        return reduce(array, axis, 2);
    }

    NDArray add(const NDArray& first, const NDArray& second)
    {
        if (first.shape() != second.shape()) {
            throw std::invalid_argument(
                "Arrays must have the same shape for addition.");
        }

        const auto& first_values = first.values();
        const auto& second_values = second.values();

        std::vector<double> result(first_values.size());

        for (std::size_t i = 0; i < result.size(); ++i) {
            result[i] = first_values[i] + second_values[i];
        }

        return NDArray(first.shape(), result);
    }

}
