#include "matrix.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void printMatrix(const std::string& label, const Matrix& matrix) {
    std::cout << label << ":\n";
    displayMatrix(matrix);
}

void printResult(const std::string& label, double value) {
    std::cout << label << value << "\n";
}
}

int main() {
    const Matrix A{{1, 2}, {3, 4}};
    const Matrix B{{5, 6}, {7, 8}};
    const Matrix rectangular{{1, 2, 3}, {4, 5, 6}};

    std::cout << "Shape helpers\n";
    std::cout << "A: " << rowsOf(A) << " x " << colsOf(A)
              << ", empty=" << std::boolalpha << isEmpty(A)
              << ", square=" << isSquare(A) << "\n\n";

    std::cout << "Construction and validation\n";
    printMatrix("Zero matrix", zeroMatrix(2, 3));
    printMatrix("Identity", identity(3));
    printMatrix("Diagonal matrix", diagonalMatrix({2, 4, 6}));
    requireNonEmpty(A);
    requireRectangular(A);
    requireSameShape(A, B);
    requireSquare(A);
    std::cout << "All validation checks passed for A and B.\n\n";

    std::cout << "Basic operations\n";
    printMatrix("A + B", addMatrices(A, B));
    printMatrix("A - B", subtractMatrices(A, B));
    printMatrix("A * B", multiplyMatrices(A, B));
    printMatrix("Hadamard product", hadamard(A, B));
    printMatrix("2 * A", scalarMultiply(2.0, A));
    printMatrix("Transpose of rectangular matrix", transposeMatrix(rectangular));
    std::cout << "\n";

    std::cout << "Comparison and properties\n";
    std::cout << "A equals A: " << matricesEqual(A, A)
              << ", symmetric: " << isSymmetric(Matrix{{1, 2}, {2, 3}})
              << ", diagonal: " << isDiagonal(Matrix{{1, 1e-8}, {0, 2}}, 1e-7) << "\n";
    printResult("trace(A) = ", trace(A));
    printResult("det(A) = ", detMatrix(A));
    std::cout << "\n";

    std::cout << "Minors, cofactors, adjugate, inverse, and powers\n";
    const Matrix C{{2, 1, 1}, {1, 3, 2}, {1, 1, 2}};
    printMatrix("Minor of C at (0, 0)", minorMatrix(C, 0, 0));
    printMatrix("Cofactor matrix", cofactorMatrix(C));
    printMatrix("Adjugate", adjugateMatrix(C));
    printMatrix("Inverse", inverseMatrix(C));
    printMatrix("C^3", matrixPower(C, 3));
    printMatrix("C^-1 via matrixPower", matrixPower(C, -1));
    std::cout << "\n";

    std::cout << "LU decomposition\n";
    const LUResult lu = luDecompose(C);
    printMatrix("L", lu.L);
    printMatrix("U", lu.U);
    std::cout << "Permutation vector: ";
    for (std::size_t index : lu.perm) std::cout << index << ' ';
    std::cout << "\nPermutation sign: " << lu.sign << "\n\n";

    std::cout << "QR decomposition\n";
    const QRResult qr = qrDecompose(rectangular);
    printMatrix("Q", qr.Q);
    printMatrix("R", qr.R);
    std::cout << "\n";

    std::cout << "Handling invalid input\n";
    try {
        requireSquare(rectangular);
    } catch (const std::invalid_argument& error) {
        std::cout << "Expected validation error: " << error.what() << "\n";
    }

    return 0;
}
