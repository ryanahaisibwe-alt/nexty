#include "part2_functions.hpp"

#include <cmath>
#include <stdexcept>

namespace part2 {

// Calculate a number raised to a power
double power(double base, double exponent) {
    double result = std::pow(base, exponent);

    if (!std::isfinite(result)) {
        throw std::overflow_error(
            "Power operation produced an invalid result"
        );
    }

    return result;
}

// Calculate the square root
double squareRoot(double x) {
    if (x < 0.0) {
        throw std::domain_error(
            "Square root of a negative number is not allowed"
        );
    }

    return std::sqrt(x);
}

// Calculate the absolute value
double absoluteValue(double x) {
    return std::fabs(x);
}

// Calculate the exponential value e^x
double exponential(double x) {
    double result = std::exp(x);

    if (!std::isfinite(result)) {
        throw std::overflow_error(
            "Exponential operation produced an invalid result"
        );
    }

    return result;
}

} // namespace part2
