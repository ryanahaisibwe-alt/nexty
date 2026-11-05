#ifndef PART3_FLOATING_POINT_AND_INVALID_INPUT_HPP
#define PART3_FLOATING_POINT_AND_INVALID_INPUT_HPP

#include <vector>

namespace part3 {

using Array = std::vector<double>;

// Checks whether a floating-point value is finite.
bool isFinite(double value);

// Checks whether all elements in an array are finite.
bool areFinite(const Array& values);

// Checks whether two floating-point values are approximately equal.
bool approximatelyEqual(
    double actual,
    double expected,
    double tolerance = 1e-9
);

} // namespace part3

#endif // PART3_FLOATING_POINT_AND_INVALID_INPUT_HPP
