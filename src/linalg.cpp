#include "addition.hpp"
#include <stdexcept>

namespace numlib
{
    double dot(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size to compute a dot product.");
        }
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    std::vector<double> matmul(const std::vector<double>& matrix, const std::vector<double>& vec, size_t rows, size_t cols) {
        if (matrix.size() != rows * cols) {
            throw std::invalid_argument("Flat matrix data size must equal rows * columns.");
        }
        if (vec.size() != cols) {
            throw std::invalid_argument("Vector size must match the number of matrix columns.");
        }
        std::vector<double> result(rows, 0.0);
        for (size_t i = 0; i < rows; ++i) {
            double sum = 0.0;
            for (size_t j = 0; j < cols; ++j) {
                sum += matrix[i * cols + j] * vec[j];
            }
            result[i] = sum;
        }
        return result;
    }
}
