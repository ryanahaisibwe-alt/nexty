#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>

// NOTE: no "using namespace std;" in a header. It would leak into every file that
// includes this one, so everything here is written with std:: instead.
using Matrix = std::vector<std::vector<double>>;

// Default tolerance for comparing floating-point results (see matricesEqual).
constexpr double kDefaultTolerance = 1e-9;

// Results of the decompositions ------------------------------------------------------------

// P*A = L*U  (see luDecompose)
//   L    unit lower triangular (1 on the diagonal, every entry has |value| <= 1)
//   U    upper triangular
//   perm row i of P*A is row perm[i] of A   (this IS the permutation matrix P, stored compactly)
//   sign +1 or -1: the parity of perm, so det(A) = sign * product of U's diagonal
struct LUResult { Matrix L; Matrix U; std::vector<std::size_t> perm; int sign; };

// A = Q*R  (see qrDecompose)
//   Q    m x m orthogonal (Q^T * Q = I)
//   R    m x n upper triangular (exact zeros below the diagonal), diagonal entries >= 0
struct QRResult { Matrix Q; Matrix R; };

// ---- Shape helpers (query only: they never throw, even on empty input) --------------------
// Shape is read from the first row, so a ragged matrix reports row 0's width.
std::size_t rowsOf(const Matrix& A);    // number of rows
std::size_t colsOf(const Matrix& A);    // number of columns (0 if there are no rows)
bool isEmpty(const Matrix& A);          // no rows, or rows with no columns
bool isSquare(const Matrix& A);         // non-empty and rows == columns

// ---- Construction -------------------------------------------------------------------------
Matrix zeroMatrix(std::size_t rows, std::size_t cols);   // rows x cols of zeros (a 0 dimension gives an empty matrix)
Matrix identity(int n);                                  // n x n identity; n must not be negative
Matrix diagonalMatrix(const std::vector<double>& v);     // square matrix with v on the diagonal

// ---- Validation (all throw std::invalid_argument) -----------------------------------------
void requireNonEmpty(const Matrix& A);                   // at least one row and one column
void requireRectangular(const Matrix& A);                // non-empty and every row has the same length
void requireSameShape(const Matrix& A, const Matrix& B); // both rectangular, same rows and columns
void requireSquare(const Matrix& A);                     // rectangular and rows == columns

// ---- Comparison and tests -----------------------------------------------------------------
// True if a and b have exactly the same shape and every pair of elements satisfies
//     |x - y| <= tol * max(1, |x|, |y|)
// i.e. an absolute tolerance for small values and a relative one for large values.
// Shape mismatch (including ragged rows) returns false rather than throwing.
// NaN is never equal to anything; infinities are equal only to the same infinity.
// Throws std::invalid_argument if tol is negative or NaN. tol = 0 means exact equality.
bool matricesEqual(const Matrix& a, const Matrix& b, double tol = kDefaultTolerance);

// True if A equals its transpose within tol (same rule as matricesEqual).
// Throws std::invalid_argument if A is not a non-empty, rectangular, square matrix or tol is bad.
bool isSymmetric(const Matrix& A, double tol = kDefaultTolerance);

// True if every off-diagonal entry satisfies |x| <= tol. Requires a square matrix
// (throws invalid_argument). A tolerance of 0.0 means exact zeros only.
bool isDiagonal(const Matrix& A, double tol = 0.0);

// ---- Basic operations (all throw std::invalid_argument on empty, ragged or mismatched input) ----
Matrix addMatrices(const Matrix& a, const Matrix& b);
Matrix subtractMatrices(const Matrix& a, const Matrix& b);
Matrix multiplyMatrices(const Matrix& a, const Matrix& b);   // (m x n)(n x p) -> m x p
Matrix hadamard(const Matrix& A, const Matrix& B);           // element-wise product
Matrix transposeMatrix(const Matrix& A);

// ---- Properties ---------------------------------------------------------------------------
double trace(const Matrix& A);      // sum of the diagonal; square matrix required
double detMatrix(const Matrix& A);  // cofactor expansion, O(n!): fine for small n only

// ---- Minors, cofactors, inverse -----------------------------------------------------------
// A with row `row` and column `col` removed. Square matrix required (invalid_argument);
// an index outside the matrix throws std::out_of_range. The minor of a 1x1 matrix is empty.
Matrix minorMatrix(const Matrix& A, int row, int col);
Matrix cofactorMatrix(const Matrix& A);   // square; the cofactor of a 1x1 matrix is {{1}}
Matrix adjugateMatrix(const Matrix& A);   // transpose of the cofactor matrix
// Inverse via the adjugate. Throws invalid_argument for a non-square matrix and
// std::runtime_error if the matrix is singular. "Singular" is judged relative to the
// matrix's own scale: |det| <= 1e-12 * (product of the row 2-norms), so a matrix such as
// diag(1e-7, 1e-7) is still invertible.
Matrix inverseMatrix(const Matrix& A);
Matrix matrixPower(const Matrix& A, int p);   // A^p by repeated squaring; p < 0 uses the inverse

// ---- Decompositions -----------------------------------------------------------------------
// Factors a square matrix with partial pivoting (largest entry in the column becomes the pivot).
// Throws std::invalid_argument if A is empty, ragged or not square. Singular matrices do NOT
// throw: they still factor, and a zero (or tiny) entry on U's diagonal tells the caller.
// Deciding "singular" is left to the caller, because the right threshold depends on the use.
// NaN or infinity in A is not checked; the factors will then contain NaN/inf.
LUResult luDecompose(const Matrix& A);

// Householder QR (full): works for any non-empty rectangular m x n shape; throws
// std::invalid_argument for an empty or ragged matrix. The sign convention diag(R) >= 0 makes
// Q's first min(m,n) columns and R unique when A has full column rank; for rank-deficient A,
// or for Q's extra columns when m > n, they are not unique. NaN or infinity is not checked.
QRResult qrDecompose(const Matrix& A);

// ---- Output -------------------------------------------------------------------------------
// Prints one row per line as |a<TAB>b<TAB>c|, default stream precision (6 significant digits).
// Throws std::invalid_argument for an empty or ragged matrix.
void displayMatrix(const Matrix& M);

// ---- Templates must be DEFINED in the header ----------------------------------------------
// The compiler needs the full body wherever the template is used, so it cannot live in
// matrix.cpp the way the ordinary functions do.
template <typename T> Matrix scalarMultiply(const T& k, const Matrix& A) {
    requireRectangular(A);

    std::size_t rows = rowsOf(A);
    std::size_t cols = colsOf(A);

    Matrix result = zeroMatrix(rows, cols);

    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            result[i][j] = k * A[i][j];
        }
    }
    return result;
}

#endif // MATRIX_HPP
