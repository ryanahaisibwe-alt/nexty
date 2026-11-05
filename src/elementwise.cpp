#include "addition.hpp"
#include <stdexcept>

namespace numlib
{
    std::vector<double> add(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size for addition.");
        }
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] + b[i];
        }
        return result;
    }

    std::vector<double> subtract(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size for subtraction.");
        }
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] - b[i];
        }
        return result;
    }

    std::vector<double> multiply(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size for multiplication.");
        }
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] * b[i];
        }
        return result;
    }

    std::vector<double> divide(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size for division.");
        }
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            if (b[i] == 0.0) {
                throw std::runtime_error("Division by zero in element-wise division.");
            }
            result[i] = a[i] / b[i];
        }
        return result;
    }

    std::vector<bool> greater_than(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size for comparison.");
        }
        std::vector<bool> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = (a[i] > b[i]);
        }
        return result;
    }

    std::vector<bool> less_than(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size for comparison.");
        }
        std::vector<bool> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = (a[i] < b[i]);
        }
        return result;
    }
}
