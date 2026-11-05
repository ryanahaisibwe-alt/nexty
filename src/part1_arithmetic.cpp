#include "part1_arithmetic.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace part1 {

// Shared internal helper to check array sizes
void checkSameSize(const Array& a, const Array& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument(
            "Arrays must have equal lengths"
        );
    }
}

// Shared internal helper to check for non-finite values
void checkFinite(const Array& values) {
    for (double value : values) {
        if (!std::isfinite(value)) {
            throw std::invalid_argument(
                "Array values must be finite"
            );
        }
    }
}

// Element-wise addition
Array add(const Array& a, const Array& b) {
    checkSameSize(a, b);
    checkFinite(a);
    checkFinite(b);

    Array result(a.size());

    for (std::size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];

        if (!std::isfinite(result[i])) {
            throw std::overflow_error(
                "Addition produced a non-finite result"
            );
        }
    }

    return result;
}

// Element-wise subtraction
Array subtract(const Array& a, const Array& b) {
    checkSameSize(a, b);
    checkFinite(a);
    checkFinite(b);

    Array result(a.size());

    for (std::size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] - b[i];

        if (!std::isfinite(result[i])) {
            throw std::overflow_error(
                "Subtraction produced a non-finite result"
            );
        }
    }

    return result;
}

// Element-wise multiplication
Array multiply(const Array& a, const Array& b) {
    checkSameSize(a, b);
    checkFinite(a);
    checkFinite(b);

    Array result(a.size());

    for (std::size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] * b[i];

        if (!std::isfinite(result[i])) {
            throw std::overflow_error(
                "Multiplication produced a non-finite result"
            );
        }
    }

    return result;
}

// Element-wise division
Array divide(const Array& a, const Array& b) {
    checkSameSize(a, b);
    checkFinite(a);
    checkFinite(b);

    Array result(a.size());

    for (std::size_t i = 0; i < a.size(); ++i) {
        if (b[i] == 0.0) {
            throw std::domain_error(
                "Division by zero is not allowed"
            );
        }

        result[i] = a[i] / b[i];

        if (!std::isfinite(result[i])) {
            throw std::overflow_error(
                "Division produced a non-finite result"
            );
        }
    }

    return result;
}

} // namespace part1
