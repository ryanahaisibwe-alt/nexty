#pragma once
#include "ndarray.hpp"
#include <cstddef>
#include <vector>
namespace array_creation {
NDArray zeros(const std::vector<std::size_t>&);
NDArray ones(const std::vector<std::size_t>&);
NDArray full(const std::vector<std::size_t>&,double);
NDArray arange(double start,double stop,double step=1.0);
NDArray concatenate(const std::vector<NDArray>&,std::size_t axis);
std::vector<NDArray> split(const NDArray&,std::size_t axis,std::size_t sections);
}
