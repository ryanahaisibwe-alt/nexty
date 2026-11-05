#include "matrix.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace std;   // fine here: a .cpp file does not leak into other files

size_t rowsOf(const Matrix& A) { return A.size(); }
size_t colsOf(const Matrix& A) { return A.empty() ? 0 : A[0].size(); }
bool isEmpty(const Matrix& A) { return rowsOf(A) == 0 || colsOf(A) == 0; }
bool isSquare(const Matrix& A) { return !isEmpty(A) && rowsOf(A) == colsOf(A); }

Matrix zeroMatrix(size_t rows, size_t cols) {
    return Matrix(rows, vector<double>(cols, 0.0));
}

void requireNonEmpty(const Matrix& A) {
    if (isEmpty(A)) {
        throw invalid_argument("Matrix must not be empty");
    }
}

void requireRectangular(const Matrix& A) {
    requireNonEmpty(A);
    const size_t columns = A[0].size();
    for (const auto& row : A) {
        if (row.size() != columns) {
            throw invalid_argument("Matrix rows must have the same number of columns");
        }
    }
}

void requireSameShape(const Matrix& A, const Matrix& B) {
    requireRectangular(A);
    requireRectangular(B);

    if (rowsOf(A) != rowsOf(B) || colsOf(A) != colsOf(B)) {
        throw invalid_argument("Matrices must have the same dimensions");
    }
}

void requireSquare(const Matrix& A) {
    requireRectangular(A);
    if (!isSquare(A))
        throw invalid_argument("Matrix must be a square matrix");
}

Matrix identity(int n) {
    if (n < 0) {
        throw invalid_argument("Matrix dimension must not be negative");
    }

    Matrix I = zeroMatrix(static_cast<size_t>(n), static_cast<size_t>(n));
    for (int i = 0; i < n; i++) I[i][i] = 1;
    return I;
}

Matrix diagonalMatrix(const vector<double>& v) {
    Matrix result = zeroMatrix(v.size(), v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        result[i][i] = v[i];
    }
    return result;
}

Matrix multiplyMatrices(const Matrix& a, const Matrix& b) {
    requireRectangular(a);
    requireRectangular(b);
    if (colsOf(a) != rowsOf(b)) {
        throw std::invalid_argument("Columns of A must equal rows of B");
    }

    size_t m = rowsOf(a);        // rows of A
    size_t n = colsOf(a);         // cols of A = rows of B
    size_t p = colsOf(b);         // cols of B

    Matrix result = zeroMatrix(m, p);

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < p; ++j) {
            for (size_t k = 0; k < n; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

LUResult luDecompose(const Matrix& A) {
    requireSquare(A);
    size_t n = rowsOf(A);
 
    // The multipliers (L) and the reduced rows (U) are built in place in a copy of A.
    Matrix work = A;
    vector<size_t> perm(n);
    for (size_t i = 0; i < n; ++i) perm[i] = i;
    int sign = 1;
 
    for (size_t k = 0; k < n; ++k) {
        // 1. Partial pivoting: pick the row at or below k with the largest |entry| in column k.
        //    Strict '>' means a tie keeps the earlier row, so we never swap without a reason.
        size_t p = k;
        for (size_t i = k + 1; i < n; ++i) {
            if (fabs(work[i][k]) > fabs(work[p][k])) p = i;
        }
 
        // 2. Swap whole rows, so multipliers already stored to the left travel with their row.
        if (p != k) {
            swap(work[k], work[p]);
            swap(perm[k], perm[p]);
            sign = -sign;
        }
 
        // 3. Eliminate below the pivot. A pivot of exactly 0 means the whole column below it
        //    is 0 too (it was the largest), so there is nothing to eliminate: skip, don't divide.
        double pivot = work[k][k];
        if (pivot == 0) continue;
 
        for (size_t i = k + 1; i < n; ++i) {
            double m = work[i][k] / pivot;       // |m| <= 1 because the pivot was the largest
            work[i][k] = m;                      // store the multiplier where the zero would go
            for (size_t j = k + 1; j < n; ++j) {
                work[i][j] -= m * work[k][j];
            }
        }
    }
 
    // Split the combined array: below the diagonal -> L, on and above -> U.
    Matrix L(n, vector<double>(n, 0.0));
    Matrix U(n, vector<double>(n, 0.0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (j < i) L[i][j] = work[i][j];
            else       U[i][j] = work[i][j];
        }
        L[i][i] = 1.0;
    }
    return LUResult{L, U, perm, sign};
}

QRResult qrDecompose(const Matrix& A) {
    requireRectangular(A);
    size_t m = rowsOf(A);
    size_t n = colsOf(A);
 
    Matrix R = A;                                   // reduced to upper triangular, step by step
    Matrix Q(m, vector<double>(m, 0.0));            // accumulates the reflections; starts as I
    for (size_t i = 0; i < m; ++i) Q[i][i] = 1.0;
 
    // Column k is cleared below the diagonal by one reflection H = I - 2*v*v^T / (v^T*v).
    // The last row never needs one (a single entry is already "triangular"), hence m - 1.
    size_t steps = min(m - 1, n);
    for (size_t k = 0; k < steps; ++k) {
        // Scale the column to O(1) before squaring, so entries near 1e200 or 1e-200 neither
        // overflow nor underflow to 0.
        double scale = 0;
        for (size_t i = k; i < m; ++i) scale = max(scale, fabs(R[i][k]));
        if (scale == 0) continue;                   // column is all zero: nothing to clear
 
        bool tailIsZero = true;
        for (size_t i = k + 1; i < m; ++i) {
            if (R[i][k] != 0) { tailIsZero = false; break; }
        }
        if (tailIsZero) continue;                   // already triangular in this column
 
        vector<double> v(m - k);
        double normSq = 0;
        for (size_t i = 0; i < m - k; ++i) {
            v[i] = R[k + i][k] / scale;
            normSq += v[i] * v[i];
        }
        double norm = sqrt(normSq);
 
        // Reflect x onto alpha * e1. Give alpha the sign OPPOSITE to x[0]: then v[0] - alpha adds
        // two same-sign numbers. The other choice subtracts nearly equal numbers when x is close
        // to the e1 axis and loses almost all precision.
        double alpha = (v[0] > 0) ? -norm : norm;
        v[0] -= alpha;
        double vtv = 0;
        for (double x : v) vtv += x * x;            // >= norm^2 > 0, so the division below is safe
 
        // R := H * R   (only columns k..n-1 can still change)
        for (size_t j = k; j < n; ++j) {
            double dot = 0;
            for (size_t i = 0; i < m - k; ++i) dot += v[i] * R[k + i][j];
            double s = 2.0 * dot / vtv;
            for (size_t i = 0; i < m - k; ++i) R[k + i][j] -= s * v[i];
        }
        // Q := Q * H   (so that A = Q * R keeps holding)
        for (size_t i = 0; i < m; ++i) {
            double dot = 0;
            for (size_t t = 0; t < m - k; ++t) dot += Q[i][k + t] * v[t];
            double s = 2.0 * dot / vtv;
            for (size_t t = 0; t < m - k; ++t) Q[i][k + t] -= s * v[t];
        }
 
        // Store the exact result rather than the rounding residue the arithmetic left behind.
        R[k][k] = alpha * scale;
        for (size_t i = k + 1; i < m; ++i) R[i][k] = 0.0;
    }
 
    // Sign convention: make every diagonal entry of R non-negative, by flipping row k of R and
    // column k of Q together (Q*R is unchanged). This makes the factorisation unique for full-rank A.
    // "0.0 - x" rather than "-x" so an exact zero stays +0.0.
    for (size_t k = 0; k < min(m, n); ++k) {
        if (R[k][k] < 0) {
            for (size_t j = 0; j < n; ++j) R[k][j] = 0.0 - R[k][j];
            for (size_t i = 0; i < m; ++i) Q[i][k] = 0.0 - Q[i][k];
        }
    }
    return QRResult{Q, R};
}

bool isSymmetric(const Matrix& A, double tol) {
    requireSquare(A);
    return matricesEqual(A, transposeMatrix(A), tol);
}

bool isDiagonal(const Matrix& A, double tol) {
    requireSquare(A);
    if (!(tol >= 0.0)) {   // written this way so a NaN tolerance is rejected too
        throw invalid_argument("Tolerance must be a non-negative number");
    }

    for (size_t i = 0; i < rowsOf(A); ++i) {
        for (size_t j = 0; j < colsOf(A); ++j) {
            if (i != j && !(fabs(A[i][j]) <= tol)) return false;
        }
    }
    return true;
}

bool matricesEqual(const Matrix& a, const Matrix& b, double tol) {
    if (!(tol >= 0)) {   // written this way so a NaN tolerance is rejected too
        throw invalid_argument("Tolerance must be a non-negative number");
    }
    if (rowsOf(a) != rowsOf(b)) return false;
 
    for (size_t i = 0; i < rowsOf(a); ++i) {
        if (a[i].size() != b[i].size()) return false;   // also protects ragged rows
 
        for (size_t j = 0; j < a[i].size(); ++j) {
            double x = a[i][j];
            double y = b[i][j];
 
            if (x == y) continue;                        // exact match, including inf == inf
            if (!isfinite(x) || !isfinite(y)) return false;   // NaN, or inf vs anything else
 
            double scale = max(1.0, max(fabs(x), fabs(y)));
            if (fabs(x - y) > tol * scale) return false;
        }
    }
    return true;
}

double detMatrix(const Matrix& A) {
    requireRectangular(A);
    requireSquare(A);
    int n = static_cast<int>(rowsOf(A));

    if (n == 1) return A[0][0];
    if (n == 2) return A[0][0]*A[1][1] - A[0][1]*A[1][0];

    double det = 0;
    int sign = 1;

    for (int col = 0; col < n; col++) {
        // build the minor: (n-1) x (n-1)
        Matrix sub = zeroMatrix(n - 1, n - 1);
        for (int i = 1; i < n; i++) {
            int c = 0;
            for (int j = 0; j < n; j++) {
                if (j == col) continue;
                sub[i-1][c++] = A[i][j];
            }
        }
        det += sign * A[0][col] * detMatrix(sub);
        sign = -sign;
    }
    return det;
}

Matrix minorMatrix(const Matrix& A, int row, int col) {
    requireSquare(A);

    const int n = static_cast<int>(A.size());
    if (row < 0 || row >= n || col < 0 || col >= n) {
        throw out_of_range("Minor row and column indices must be within the matrix");
    }

    Matrix sub = zeroMatrix(n - 1, n - 1);
    int r = 0;
    for (int i = 0; i < n; ++i) {
        if (i == row) continue;
        int c = 0;
        for (int j = 0; j < n; ++j) {
            if (j == col) continue;
            sub[r][c++] = A[i][j];
        }
        ++r;
    }
    return sub;
}

Matrix cofactorMatrix(const Matrix& A) {
    requireSquare(A);
    const size_t n = rowsOf(A);
    Matrix result = zeroMatrix(n, n);

    if (n == 1) {
        result[0][0] = 1.0;
        return result;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            const double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
            result[i][j] = sign * detMatrix(minorMatrix(A, static_cast<int>(i), static_cast<int>(j)));
        }
    }
    return result;
}

Matrix adjugateMatrix(const Matrix& A) {
    requireSquare(A);
    return transposeMatrix(cofactorMatrix(A));
}

Matrix inverseMatrix(const Matrix& A) {
    requireSquare(A);
    const double det = detMatrix(A);

    // Judge "singular" relative to the matrix's own scale, not against a fixed number.
    // Hadamard's inequality gives |det| <= product of the row 2-norms, so
    // |det| / scale lies in [0, 1] whatever units the entries are in.
    double scale = 1.0;
    for (const auto& row : A) {
        double sumSq = 0;
        for (double x : row) sumSq += x * x;
        scale *= sqrt(sumSq);
    }
    const double relTol = 1e-12;
    if (scale == 0 || fabs(det) <= relTol * scale)
        throw runtime_error("Matrix is singular: no inverse exists");

    Matrix adjugate = adjugateMatrix(A);
    for (auto& row : adjugate) {
        for (double& value : row) value /= det;
    }
    return adjugate;
}

Matrix addMatrices(const Matrix& a, const Matrix& b) {
    requireRectangular(a);
    requireRectangular(b);
    requireSameShape(a, b);

    size_t rows = rowsOf(a);
    size_t cols = colsOf(a);

    Matrix result = zeroMatrix(rows, cols);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
    return result;
}

Matrix subtractMatrices(const Matrix& a, const Matrix& b){
    requireRectangular(a);
    requireRectangular(b);
    requireSameShape(a, b);

    size_t rows = rowsOf(a);
    size_t cols = colsOf(a);

    Matrix result = zeroMatrix(rows, cols);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = a[i][j] - b[i][j];
        }
    }
    return result;
}

Matrix transposeMatrix(const Matrix& A){
    requireRectangular(A);
    size_t cols = rowsOf(A);
    size_t rows = colsOf(A);
    Matrix result = zeroMatrix(rows, cols);
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            result[i][j] = A[j][i];
        }
    }
    return result;
}

Matrix hadamard(const Matrix& A, const Matrix& B){
    requireRectangular(A);
    requireRectangular(B);
    requireSameShape(A, B);
    int rows = static_cast<int>(rowsOf(A));
        int cols = static_cast<int>(colsOf(A));
        Matrix result = zeroMatrix(rows, cols);

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            result[i][j] = A[i][j] * B[i][j]; 
        }
    }
    return result;
}

Matrix matrixPower(const Matrix& A, int p) {
    requireRectangular(A);
    requireSquare(A);

    Matrix base = A;
    unsigned int exponent;

    if (p < 0) {
        base = inverseMatrix(A);
        // Avoid negating INT_MIN, which would overflow.
        exponent = static_cast<unsigned int>(-(p + 1)) + 1;
    } else {
        exponent = static_cast<unsigned int>(p);
    }

    Matrix result = identity(static_cast<int>(A.size()));
    while (exponent > 0) {
        if (exponent & 1U) result = multiplyMatrices(result, base);
        exponent >>= 1U;
        if (exponent > 0) base = multiplyMatrices(base, base);
    }
    return result;
}

double trace(const Matrix& A){
    requireRectangular(A);
    requireSquare(A);

    double trace = 0;

    for(size_t i = 0; i < A.size(); i++) trace += A[i][i];
    return trace;
}

void displayMatrix(const Matrix& M) {
    requireRectangular(M);
    for (const auto& row : M) {
        cout << "|";
        for (size_t j = 0; j < row.size(); ++j) {
            if (j > 0) cout << "\t";
            cout << row[j];
        }
        cout << "|\n";
    }
}
