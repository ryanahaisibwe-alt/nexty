#include "part3_floating_Point_and_Invalid_Input.hpp"

#include <cmath>

namespace part3 {

// Checks whether a single value is finite.
bool isFinite(double value) {
    return std::isfinite(value);
}

// Checks whether all values in an array are finite.
bool areFinite(const Array& values) {
    for (double value : values) {
        if (!isFinite(value)) {
            return false;
        }
    }

    return true;
}

// Compares two floating-point values using a tolerance.
bool approximatelyEqual(
    double actual,
    double expected,
    double tolerance
) {
    if (!isFinite(actual) || !isFinite(expected) ||
        !isFinite(tolerance) || tolerance < 0.0) {
        return false;
    }

    return std::fabs(actual - expected) <= tolerance;
}

} // namespace part3
