#pragma once
#include "ndarray.hpp"
#include <cstddef>
#include <vector>
namespace indexing {
double get(const NDArray&, const std::vector<std::size_t>&);
void set(NDArray&, const std::vector<std::size_t>&, double);
std::vector<std::size_t> shape(const NDArray&);
std::size_t dimensions(const NDArray&);
std::size_t size(const NDArray&);
NDArray slice(const NDArray&, std::size_t axis, std::size_t start,
              std::size_t stop, std::size_t step = 1);
NDArray reshape(const NDArray&, const std::vector<std::size_t>&);
}
