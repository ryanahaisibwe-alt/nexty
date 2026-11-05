// Verification harness for matrix.hpp / matrix.cpp (the library is NOT modified by it).
// Build: g++ -std=c++17 -g -fsanitize=address,undefined test_matrix.cpp matrix.cpp -o test_matrix
// Usage: ./test_matrix            -> run all normal + invalid-input tests
//        ./test_matrix edge NAME  -> run one risky edge case in isolation
#include "matrix.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

static int passed = 0, failed = 0;

static bool close(const Matrix& a, const Matrix& b, double eps = 1e-9) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].size() != b[i].size()) return false;
        for (size_t j = 0; j < a[i].size(); ++j)
            if (!(fabs(a[i][j] - b[i][j]) <= eps)) return false;   // written so NaN counts as "different"
    }
    return true;
}
static void check(const string& name, bool ok) {
    cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
    ok ? ++passed : ++failed;
}
template <typename F>
static bool throwsAny(F f) { try { f(); } catch (...) { return true; } return false; }
template <typename F>
static bool throwsRuntime(F f) { try { f(); } catch (const runtime_error&) { return true; } catch (...) { return false; } return false; }
// Stricter: passes only for the documented exception type, so a stray
// out_of_range / bad_alloc cannot masquerade as correct validation.
template <typename F>
static bool throwsInvalid(F f) { try { f(); } catch (const invalid_argument&) { return true; } catch (...) { return false; } return false; }

int main(int argc, char** argv) {
    if (argc == 3 && string(argv[1]) == "edge") {
        string c = argv[2];
        if (c == "scalar_empty")   { scalarMultiply(2.0, Matrix{}); }
        if (c == "transpose_empty"){ transposeMatrix(Matrix{}); }
        if (c == "trace_empty")    { trace(Matrix{}); }
        if (c == "hadamard_empty") { hadamard(Matrix{}, Matrix{}); }
        if (c == "det_nonsquare")  { cout << detMatrix({{1,2,3},{4,5,6}}) << "\n"; }
        if (c == "inv_nonsquare")  { auto r = inverseMatrix({{1,2,3},{4,5,6}}); cout << r.size() << "x" << r[0].size() << "\n"; }
        if (c == "power_nonsquare"){ auto r = matrixPower({{1,2,3},{4,5,6}}, 2); cout << r.size() << "\n"; }
        if (c == "det_empty")      { cout << detMatrix(Matrix{}) << "\n"; }
        if (c == "identity_zero")  { cout << identity(0).size() << "\n"; }
        if (c == "identity_negative") { identity(-1); }
        if (c == "inv_badscale")   { // tiny but perfectly invertible matrix
            Matrix A = {{1e-7,0},{0,1e-7}};
            cout << "det=" << detMatrix(A) << "\n";
            inverseMatrix(A);
            cout << "inverted ok\n";
        }
        return 0;
    }

    Matrix A = {{1,2},{3,4}}, B = {{5,6},{7,8}};
    Matrix R = {{1,2,3},{4,5,6}};       // 2x3
    Matrix C = {{1,2},{3,4},{5,6}};     // 3x2

    // identity
    check("identity(3)", close(identity(3), {{1,0,0},{0,1,0},{0,0,1}}));
    check("identity(0) is empty", identity(0).empty());
    check("identity(-1) throws invalid_argument", throwsInvalid([]{ identity(-1); }));
    // multiply
    check("multiply 2x2", close(multiplyMatrices(A,B), {{19,22},{43,50}}));
    check("multiply 2x3 * 3x2", close(multiplyMatrices(R,C), {{22,28},{49,64}}));
    check("multiply by identity", close(multiplyMatrices(A, identity(2)), A));
    check("multiply bad dims throws", throwsAny([&]{ multiplyMatrices(R, R); }));
    // add / subtract
    check("add", close(addMatrices(A,B), {{6,8},{10,12}}));
    check("add bad dims throws", throwsAny([&]{ addMatrices(A, R); }));
    check("subtract", close(subtractMatrices(B,A), {{4,4},{4,4}}));
    check("subtract bad dims throws", throwsAny([&]{ subtractMatrices(A, R); }));
    // scalar
    check("scalar double", close(scalarMultiply(2.5, A), {{2.5,5},{7.5,10}}));
    check("scalar int", close(scalarMultiply(3, A), {{3,6},{9,12}}));
    // transpose
    check("transpose 2x3", close(transposeMatrix(R), {{1,4},{2,5},{3,6}}));
    check("transpose twice = original", close(transposeMatrix(transposeMatrix(R)), R));
    // hadamard
    check("hadamard", close(hadamard(A,B), {{5,12},{21,32}}));
    check("hadamard bad dims throws", throwsAny([&]{ hadamard(A, R); }));
    // trace
    check("trace 2x2", fabs(trace(A) - 5) < 1e-12);
    check("trace non-square throws", throwsAny([&]{ trace(R); }));
    // determinant
    check("det 1x1", fabs(detMatrix({{7}}) - 7) < 1e-12);
    check("det 2x2", fabs(detMatrix(A) - (-2)) < 1e-12);
    check("det 3x3 known value -306", fabs(detMatrix({{6,1,1},{4,-2,5},{2,8,7}}) - (-306)) < 1e-9);
    check("det 4x4 identity", fabs(detMatrix(identity(4)) - 1) < 1e-12);
    check("det singular = 0", fabs(detMatrix({{1,2,3},{4,5,6},{7,8,9}})) < 1e-9);
    check("det non-square throws invalid_argument", throwsInvalid([&]{ detMatrix(R); }));
    // inverse
    check("inverse 2x2", close(inverseMatrix(A), {{-2,1},{1.5,-0.5}}));
    Matrix M3 = {{2,0,1},{1,3,2},{1,1,2}};   // det = 6
    check("inverse 3x3: M*M^-1 = I", close(multiplyMatrices(M3, inverseMatrix(M3)), identity(3), 1e-9));
    check("inverse 1x1", close(inverseMatrix({{4}}), {{0.25}}));
    check("inverse singular throws runtime_error", throwsRuntime([&]{ inverseMatrix({{1,2},{2,4}}); }));
    check("inverse non-square throws invalid_argument", throwsInvalid([&]{ inverseMatrix(R); }));
    // power
    check("power 0 = I", close(matrixPower(A,0), identity(2)));
    check("power 1", close(matrixPower(A,1), A));
    check("power 3", close(matrixPower(A,3), multiplyMatrices(A, multiplyMatrices(A,A))));
    check("power -1 = inverse", close(matrixPower(A,-1), inverseMatrix(A)));
    check("power -2", close(matrixPower(A,-2), multiplyMatrices(inverseMatrix(A), inverseMatrix(A))));
    check("power non-square throws", throwsAny([&]{ matrixPower(R, 2); }));
    // minor
    check("minor(0,0) of 3x3", close(minorMatrix({{1,2,3},{4,5,6},{7,8,9}},0,0), {{5,6},{8,9}}));
    check("minor(1,2) of 3x3", close(minorMatrix({{1,2,3},{4,5,6},{7,8,9}},1,2), {{1,2},{7,8}}));

    // ---- Hardening round 1: scalarMultiply, transposeMatrix, hadamard, trace ----
    Matrix E0 = {};          // no rows
    Matrix E1 = {{}};        // one row, no columns
    // empty input must throw invalid_argument (not crash)
    check("scalar empty (no rows) throws invalid_argument", throwsInvalid([&]{ scalarMultiply(2.0, E0); }));
    check("scalar empty (no cols) throws invalid_argument", throwsInvalid([&]{ scalarMultiply(2.0, E1); }));
    check("transpose empty (no rows) throws invalid_argument", throwsInvalid([&]{ transposeMatrix(E0); }));
    check("transpose empty (no cols) throws invalid_argument", throwsInvalid([&]{ transposeMatrix(E1); }));
    check("hadamard empty/empty throws invalid_argument", throwsInvalid([&]{ hadamard(E0, E0); }));
    check("hadamard empty/non-empty throws invalid_argument", throwsInvalid([&]{ hadamard(E0, A); }));
    check("hadamard non-empty/empty throws invalid_argument", throwsInvalid([&]{ hadamard(A, E0); }));
    check("hadamard no-cols throws invalid_argument", throwsInvalid([&]{ hadamard(E1, E1); }));
    check("trace empty (no rows) throws invalid_argument", throwsInvalid([&]{ trace(E0); }));
    check("trace empty (no cols) throws invalid_argument", throwsInvalid([&]{ trace(E1); }));
    check("trace non-square throws invalid_argument", throwsInvalid([&]{ trace(R); }));
    // behaviour that must NOT change
    check("scalar by 0 gives zeros", close(scalarMultiply(0, A), {{0,0},{0,0}}));
    check("scalar negative", close(scalarMultiply(-1.5, A), {{-1.5,-3},{-4.5,-6}}));
    check("scalar on 1x1", close(scalarMultiply(4, Matrix{{2}}), {{8}}));
    check("scalar on non-square 2x3", close(scalarMultiply(2, R), {{2,4,6},{8,10,12}}));
    check("scalar does not modify input", close(A, {{1,2},{3,4}}));
    check("transpose 1x1", close(transposeMatrix({{9}}), {{9}}));
    check("transpose row vector 1x3", close(transposeMatrix({{1,2,3}}), {{1},{2},{3}}));
    check("transpose column vector 3x1", close(transposeMatrix({{1},{2},{3}}), {{1,2,3}}));
    check("transpose square", close(transposeMatrix(A), {{1,3},{2,4}}));
    check("(AB)^T = B^T A^T", close(transposeMatrix(multiplyMatrices(A,B)),
                                    multiplyMatrices(transposeMatrix(B), transposeMatrix(A))));
    check("hadamard non-square 2x3", close(hadamard(R, R), {{1,4,9},{16,25,36}}));
    check("hadamard with zeros", close(hadamard(A, {{0,0},{0,0}}), {{0,0},{0,0}}));
    check("hadamard same rows, different cols throws", throwsInvalid([&]{ hadamard(R, A); }));
    check("hadamard same cols, different rows throws", throwsInvalid([&]{ hadamard(C, R); }));
    check("trace 1x1", fabs(trace({{3.5}}) - 3.5) < 1e-12);
    check("trace 3x3", fabs(trace({{1,2,3},{4,5,6},{7,8,9}}) - 15) < 1e-12);
    check("trace of identity(5) = 5", fabs(trace(identity(5)) - 5) < 1e-12);
    check("trace(A+B) = trace(A)+trace(B)", fabs(trace(addMatrices(A,B)) - (trace(A)+trace(B))) < 1e-12);
    check("trace(AB) = trace(BA)", fabs(trace(multiplyMatrices(A,B)) - trace(multiplyMatrices(B,A))) < 1e-12);

    // ---- Hardening round 2: detMatrix, inverseMatrix ----
    Matrix R3x2 = C;                       // 3x2
    Matrix A4 = {{1,2,3,4},{5,6,7,8},{2,6,4,8},{3,1,1,2}};   // det = 72 (numpy)
    Matrix UT = {{2,5,7},{0,3,1},{0,0,4}};                    // det = 24
    Matrix S3 = {{0,1,2},{3,4,5},{6,7,9}};                    // det = -3
    Matrix S3swap = {{3,4,5},{0,1,2},{6,7,9}};                // rows 0,1 swapped: det = +3

    // detMatrix: invalid input
    check("det 3x2 throws invalid_argument", throwsInvalid([&]{ detMatrix(R3x2); }));
    check("det empty (no rows) throws invalid_argument", throwsInvalid([&]{ detMatrix(E0); }));
    check("det empty (no cols) throws invalid_argument", throwsInvalid([&]{ detMatrix(E1); }));
    check("det 3x4 throws invalid_argument", throwsInvalid([&]{ detMatrix({{1,2,3,4},{5,6,7,8},{9,10,11,12}}); }));
    // detMatrix: values
    check("det 4x4 known = 72", fabs(detMatrix(A4) - 72) < 1e-9);
    check("det upper triangular = product of diagonal (24)", fabs(detMatrix(UT) - 24) < 1e-9);
    check("det 3x3 = -3", fabs(detMatrix(S3) - (-3)) < 1e-9);
    check("det row swap flips sign", fabs(detMatrix(S3swap) - 3) < 1e-9);
    check("det identity(5) = 1", fabs(detMatrix(identity(5)) - 1) < 1e-12);
    check("det zero matrix = 0", fabs(detMatrix({{0,0},{0,0}})) < 1e-12);
    check("det(A^T) = det(A)", fabs(detMatrix(transposeMatrix(A4)) - detMatrix(A4)) < 1e-9);
    check("det(AB) = det(A)det(B)", fabs(detMatrix(multiplyMatrices(A,B)) - detMatrix(A)*detMatrix(B)) < 1e-9);
    check("det(kA) = k^n det(A), n=4", fabs(detMatrix(scalarMultiply(2, A4)) - 16*72) < 1e-6);
    check("det does not modify input", close(S3, {{0,1,2},{3,4,5},{6,7,9}}));

    // inverseMatrix: invalid input
    check("inverse 3x2 throws invalid_argument", throwsInvalid([&]{ inverseMatrix(R3x2); }));
    check("inverse empty (no rows) throws invalid_argument", throwsInvalid([&]{ inverseMatrix(E0); }));
    check("inverse empty (no cols) throws invalid_argument", throwsInvalid([&]{ inverseMatrix(E1); }));
    // inverseMatrix: singular detection
    check("inverse zero matrix throws runtime_error", throwsRuntime([&]{ inverseMatrix({{0,0},{0,0}}); }));
    check("inverse 1x1 zero throws runtime_error", throwsRuntime([&]{ inverseMatrix({{0}}); }));
    check("inverse zero row throws runtime_error", throwsRuntime([&]{ inverseMatrix({{1,2},{0,0}}); }));
    check("inverse 3x3 singular (1..9) throws runtime_error", throwsRuntime([&]{ inverseMatrix({{1,2,3},{4,5,6},{7,8,9}}); }));
    check("inverse SMALL-scale singular throws runtime_error",
          throwsRuntime([&]{ inverseMatrix({{1e-7,2e-7},{2e-7,4e-7}}); }));
    check("inverse LARGE-scale singular throws runtime_error",
          throwsRuntime([&]{ inverseMatrix({{1e7,2e7},{2e7,4e7}}); }));
    check("inverse nearly dependent rows (rel. 1e-14) throws runtime_error",
          throwsRuntime([&]{ inverseMatrix({{1,2},{1,2+1e-14}}); }));
    // inverseMatrix: scale invariance (the bug found in review)
    check("inverse tiny-scale invertible diag(1e-7,1e-7)", close(inverseMatrix({{1e-7,0},{0,1e-7}}), {{1e7,0},{0,1e7}}, 1e-3));
    check("inverse tiny-scale invertible, relative error small",
          fabs(inverseMatrix({{1e-7,0},{0,1e-7}})[0][0] / 1e7 - 1) < 1e-12);
    check("inverse large-scale invertible diag(1e7,1e7)", fabs(inverseMatrix({{1e7,0},{0,1e7}})[0][0] / 1e-7 - 1) < 1e-12);
    check("inverse mixed-scale diag(1e8,1e-8)", fabs(inverseMatrix({{1e8,0},{0,1e-8}})[1][1] / 1e8 - 1) < 1e-12);
    check("inverse well-posed but small det still works {{1,1},{1,1+1e-9}}",
          !throwsAny([&]{ inverseMatrix({{1,1},{1,1+1e-9}}); }));
    // inverseMatrix: values and identities
    check("inverse diag(2,4,5) = diag(.5,.25,.2)", close(inverseMatrix({{2,0,0},{0,4,0},{0,0,5}}), {{0.5,0,0},{0,0.25,0},{0,0,0.2}}));
    check("inverse 4x4 known (numpy) [0][0] = -1/6", fabs(inverseMatrix(A4)[0][0] - (-1.0/6)) < 1e-12);
    check("inverse 4x4 known (numpy) [3][1] = -13/36", fabs(inverseMatrix(A4)[3][1] - (-13.0/36)) < 1e-12);
    check("A * A^-1 = I (4x4)", close(multiplyMatrices(A4, inverseMatrix(A4)), identity(4), 1e-9));
    check("A^-1 * A = I (4x4)", close(multiplyMatrices(inverseMatrix(A4), A4), identity(4), 1e-9));
    check("(A^-1)^-1 = A (4x4)", close(inverseMatrix(inverseMatrix(A4)), A4, 1e-9));
    check("(A^T)^-1 = (A^-1)^T", close(inverseMatrix(transposeMatrix(A4)), transposeMatrix(inverseMatrix(A4)), 1e-9));
    check("det(A^-1) = 1/det(A)", fabs(detMatrix(inverseMatrix(A4)) - 1.0/72) < 1e-12);
    check("inverse does not modify input", close(A4, {{1,2,3,4},{5,6,7,8},{2,6,4,8},{3,1,1,2}}));

    // ---- Shape helpers: rowsOf, colsOf, isEmpty, isSquare ----
    // rowsOf / colsOf
    check("rowsOf 2x3 = 2", rowsOf(R) == 2);
    check("colsOf 2x3 = 3", colsOf(R) == 3);
    check("rowsOf 3x2 = 3", rowsOf(C) == 3);
    check("colsOf 3x2 = 2", colsOf(C) == 2);
    check("rowsOf/colsOf 1x1", rowsOf(Matrix{{5}}) == 1 && colsOf(Matrix{{5}}) == 1);
    check("row vector 1x4", rowsOf(Matrix{{1,2,3,4}}) == 1 && colsOf(Matrix{{1,2,3,4}}) == 4);
    check("column vector 4x1", rowsOf(Matrix{{1},{2},{3},{4}}) == 4 && colsOf(Matrix{{1},{2},{3},{4}}) == 1);
    check("rowsOf/colsOf of {} = 0, 0", rowsOf(E0) == 0 && colsOf(E0) == 0);
    check("{{}} has 1 row and 0 columns", rowsOf(E1) == 1 && colsOf(E1) == 0);
    check("rowsOf/colsOf never throw on empty input",
          !throwsAny([&]{ rowsOf(E0); colsOf(E0); rowsOf(E1); colsOf(E1); }));
    check("colsOf returns size_t-compatible value (no negative)", colsOf(E0) == size_t(0));
    // isEmpty
    check("isEmpty({}) true", isEmpty(E0));
    check("isEmpty({{}}) true", isEmpty(E1));
    check("isEmpty(1x1) false", !isEmpty(Matrix{{0}}));
    check("isEmpty(2x3) false", !isEmpty(R));
    check("isEmpty(zero matrix) false (zeros are not empty)", !isEmpty(Matrix{{0,0},{0,0}}));
    // isSquare
    check("isSquare 1x1 true", isSquare(Matrix{{7}}));
    check("isSquare 2x2 true", isSquare(A));
    check("isSquare identity(6) true", isSquare(identity(6)));
    check("isSquare 2x3 false", !isSquare(R));
    check("isSquare 3x2 false", !isSquare(C));
    check("isSquare {} false (empty is not square)", !isSquare(E0));
    check("isSquare {{}} false", !isSquare(E1));
    check("isSquare never throws on empty input", !throwsAny([&]{ isSquare(E0); isSquare(E1); }));
    // helpers agree with the require* functions
    check("isEmpty agrees with requireNonEmpty (empty)", isEmpty(E0) == throwsInvalid([&]{ requireNonEmpty(E0); }));
    check("isEmpty agrees with requireNonEmpty (non-empty)", isEmpty(R) == throwsInvalid([&]{ requireNonEmpty(R); }));
    check("isSquare agrees with requireSquare (2x3)", isSquare(R) == !throwsInvalid([&]{ requireSquare(R); }));
    check("isSquare agrees with requireSquare (2x2)", isSquare(A) == !throwsInvalid([&]{ requireSquare(A); }));
    check("isSquare agrees with requireSquare ({})", isSquare(E0) == !throwsInvalid([&]{ requireSquare(E0); }));
    // shapes of results line up with the helpers (cross-checks the helpers on real data)
    check("transpose swaps rows and cols", rowsOf(transposeMatrix(R)) == colsOf(R) && colsOf(transposeMatrix(R)) == rowsOf(R));
    check("product shape = rows(a) x cols(b)", rowsOf(multiplyMatrices(R, C)) == rowsOf(R) && colsOf(multiplyMatrices(R, C)) == colsOf(C));
    check("inverse of square is square", isSquare(inverseMatrix(A4)));
    check("identity(n) is n x n", rowsOf(identity(4)) == 4 && colsOf(identity(4)) == 4);
    check("helpers do not modify input", close(R, {{1,2,3},{4,5,6}}));

    // ---- isDiagonal ----
    check("diagonal: exact diagonal matrix", isDiagonal(Matrix{{1,0,0},{0,2,0},{0,0,3}}));
    check("diagonal: default tolerance requires exact off-diagonal zeros",
          !isDiagonal(Matrix{{1,1e-12},{0,2}}));
    check("diagonal: zero tolerance accepts exact zeros", isDiagonal(Matrix{{1,0},{0,2}}, 0.0));
    check("diagonal: custom tolerance accepts small off-diagonal entries",
          isDiagonal(Matrix{{1,1e-8},{-1e-8,2}}, 1e-8));
    check("diagonal: custom tolerance rejects larger off-diagonal entry",
          !isDiagonal(Matrix{{1,1.1e-8},{0,2}}, 1e-8));
    check("diagonal: negative tolerance throws invalid_argument",
          throwsInvalid([&]{ isDiagonal(Matrix{{1,0},{0,2}}, -1e-9); }));
    check("diagonal: NaN tolerance throws invalid_argument",
          throwsInvalid([&]{ isDiagonal(Matrix{{1,0},{0,2}}, NAN); }));
    check("diagonal: non-square matrix throws invalid_argument",
          throwsInvalid([&]{ isDiagonal(R); }));
    check("diagonal: empty matrix throws invalid_argument",
          throwsInvalid([&]{ isDiagonal(E0); }));
    check("diagonal: NaN off-diagonal is not diagonal",
          !isDiagonal(Matrix{{1,NAN},{0,2}}, 1.0));

    // ---- matricesEqual ----
    // NOTE: close() above stays as an INDEPENDENT checker on purpose. If every test used
    // matricesEqual and it were wrong (say, always true), every test would pass vacuously.
    Matrix A2 = {{1,2},{3,4}};
    Matrix SYM = {{1,2},{2,3}};
    // identical / different
    check("eq: same matrix", matricesEqual(A2, A2));
    check("eq: equal copy", matricesEqual(A2, Matrix{{1,2},{3,4}}));
    check("eq: different values", !matricesEqual(A2, B));
    check("eq: differs in last element only", !matricesEqual(A2, Matrix{{1,2},{3,5}}));
    check("eq: differs in first element only", !matricesEqual(A2, Matrix{{9,2},{3,4}}));
    check("eq: 1x1 equal", matricesEqual(Matrix{{5}}, Matrix{{5}}));
    check("eq: 1x1 different", !matricesEqual(Matrix{{5}}, Matrix{{6}}));
    // tolerance
    check("eq: tol=0 identical", matricesEqual(A2, A2, 0.0));
    check("eq: tol=0 rejects 1e-15 difference", !matricesEqual(A2, Matrix{{1+1e-15,2},{3,4}}, 0.0));
    check("eq: within default tol (1e-10)", matricesEqual(A2, Matrix{{1+1e-10,2},{3,4}}));
    check("eq: outside default tol (1e-8)", !matricesEqual(A2, Matrix{{1+1e-8,2},{3,4}}));
    check("eq: custom tol accepts 1e-8 when tol=1e-6", matricesEqual(A2, Matrix{{1+1e-8,2},{3,4}}, 1e-6));
    check("eq: custom tol rejects 1e-3 when tol=1e-6", !matricesEqual(A2, Matrix{{1+1e-3,2},{3,4}}, 1e-6));
    check("eq: tol applies to negative differences too", matricesEqual(A2, Matrix{{1-1e-10,2},{3,4}}));
    check("eq: default argument = kDefaultTolerance",
          matricesEqual(A2, Matrix{{1+5e-10,2},{3,4}}) == matricesEqual(A2, Matrix{{1+5e-10,2},{3,4}}, kDefaultTolerance));
    // relative at large scale, absolute floor near zero
    check("eq: large values, tiny relative gap (1e12 vs 1e12+1)", matricesEqual(Matrix{{1e12}}, Matrix{{1e12+1}}));
    check("eq: large values, big relative gap (1e-6 relative)", !matricesEqual(Matrix{{1e12}}, Matrix{{1e12*(1+1e-6)}}));
    check("eq: near zero accepted (0 vs 1e-10)", matricesEqual(Matrix{{0}}, Matrix{{1e-10}}));
    check("eq: near zero rejected (0 vs 1e-8)", !matricesEqual(Matrix{{0}}, Matrix{{1e-8}}));
    check("eq: negative values", matricesEqual(Matrix{{-3.5}}, Matrix{{-3.5}}) && !matricesEqual(Matrix{{-3.5}}, Matrix{{3.5}}));
    check("eq: -0.0 equals 0.0", matricesEqual(Matrix{{-0.0}}, Matrix{{0.0}}, 0.0));
    // symmetry of the relation
    check("eq: symmetric (equal case)", matricesEqual(A2, Matrix{{1+1e-10,2},{3,4}}) == matricesEqual(Matrix{{1+1e-10,2},{3,4}}, A2));
    check("eq: symmetric (unequal case)", matricesEqual(A2, B) == matricesEqual(B, A2));
    check("eq: symmetric (large scale)", matricesEqual(Matrix{{1e12}}, Matrix{{1e12+1}}) == matricesEqual(Matrix{{1e12+1}}, Matrix{{1e12}}));
    // shape mismatch -> false, never throws, never reads out of bounds
    check("eq: 2x2 vs 2x3 false", !matricesEqual(A2, R));
    check("eq: 2x3 vs 3x2 false", !matricesEqual(R, C));
    check("eq: 2x3 vs 2x2 false", !matricesEqual(R, A2));
    check("eq: 1x1 vs 2x2 false", !matricesEqual(Matrix{{1}}, A2));
    check("eq: row vector vs column vector false", !matricesEqual(Matrix{{1,2,3}}, Matrix{{1},{2},{3}}));
    check("eq: shape mismatch does not throw", !throwsAny([&]{ matricesEqual(A2, R); matricesEqual(R, C); }));
    // empty
    check("eq: {} equals {}", matricesEqual(E0, E0));
    check("eq: {{}} equals {{}}", matricesEqual(E1, E1));
    check("eq: {} differs from {{}} (0x0 vs 1x0)", !matricesEqual(E0, E1));
    check("eq: {} differs from 1x1", !matricesEqual(E0, Matrix{{0}}));
    check("eq: empty does not throw", !throwsAny([&]{ matricesEqual(E0, E0); matricesEqual(E1, A2); matricesEqual(E0, E1); }));
    // ragged rows (must not crash)
    Matrix RAG = {{1,2},{3}};
    check("eq: ragged vs well-formed false", !matricesEqual(RAG, A2));
    check("eq: well-formed vs ragged false", !matricesEqual(A2, RAG));
    check("eq: ragged vs identical ragged copy true", matricesEqual(RAG, Matrix{{1,2},{3}}));
    check("eq: ragged rows of different lengths false", !matricesEqual(Matrix{{1},{2,3}}, Matrix{{1,9},{2}}));
    check("eq: ragged does not throw", !throwsAny([&]{ matricesEqual(RAG, A2); matricesEqual(A2, RAG); }));
    // NaN and infinity
    check("eq: NaN never equals NaN", !matricesEqual(Matrix{{NAN}}, Matrix{{NAN}}));
    check("eq: matrix containing NaN is not equal to itself", !matricesEqual(Matrix{{1,NAN}}, Matrix{{1,NAN}}));
    check("eq: NaN vs number false", !matricesEqual(Matrix{{NAN}}, Matrix{{1}}));
    check("eq: number vs NaN false", !matricesEqual(Matrix{{1}}, Matrix{{NAN}}));
    check("eq: inf equals inf", matricesEqual(Matrix{{INFINITY}}, Matrix{{INFINITY}}));
    check("eq: inf differs from -inf", !matricesEqual(Matrix{{INFINITY}}, Matrix{{-INFINITY}}));
    check("eq: inf differs from 1", !matricesEqual(Matrix{{INFINITY}}, Matrix{{1}}));
    check("eq: inf differs from huge finite", !matricesEqual(Matrix{{INFINITY}}, Matrix{{1e308}}));
    check("eq: huge opposite signs differ", !matricesEqual(Matrix{{1e308}}, Matrix{{-1e308}}));
    // bad tolerance
    check("eq: negative tol throws invalid_argument", throwsInvalid([&]{ matricesEqual(A2, A2, -1e-9); }));
    check("eq: NaN tol throws invalid_argument", throwsInvalid([&]{ matricesEqual(A2, A2, NAN); }));
    check("eq: bad tol throws even for mismatched shapes", throwsInvalid([&]{ matricesEqual(A2, R, -1.0); }));
    // used the way the library will use it
    check("eq: A * A^-1 equals identity", matricesEqual(multiplyMatrices(A2, inverseMatrix(A2)), identity(2)));
    check("eq: A + B equals B + A", matricesEqual(addMatrices(A2, B), addMatrices(B, A2)));
    check("eq: (AB)^T equals B^T A^T", matricesEqual(transposeMatrix(multiplyMatrices(A2, B)),
                                                      multiplyMatrices(transposeMatrix(B), transposeMatrix(A2))));
    check("eq: non-symmetric A differs from A^T", !matricesEqual(A2, transposeMatrix(A2)));
    check("eq: symmetric matrix equals its transpose", matricesEqual(SYM, transposeMatrix(SYM)));
    check("eq: A - A equals zero matrix", matricesEqual(subtractMatrices(A2, A2), Matrix{{0,0},{0,0}}));
    check("eq: agrees with independent close() on a mixed bag",
          matricesEqual(A4, inverseMatrix(inverseMatrix(A4))) == close(A4, inverseMatrix(inverseMatrix(A4))) &&
          matricesEqual(A2, B) == close(A2, B));
    check("eq: does not modify inputs", close(A2, {{1,2},{3,4}}) && close(B, {{5,6},{7,8}}));

    // ---- luDecompose: PA = LU with partial pivoting ----
    // Independent helpers: nothing here uses luDecompose, so they cannot hide its bugs.
    auto permuteRows = [](const Matrix& M, const vector<size_t>& p) {
        Matrix r; for (size_t i : p) r.push_back(M[i]); return r; };
    auto parityOf = [](const vector<size_t>& p) {          // +1 even, -1 odd (counts inversions)
        int inv = 0;
        for (size_t i = 0; i < p.size(); ++i) for (size_t j = i + 1; j < p.size(); ++j) if (p[i] > p[j]) ++inv;
        return inv % 2 == 0 ? 1 : -1; };
    auto isPermutation = [](const vector<size_t>& p) {
        vector<int> seen(p.size(), 0);
        for (size_t v : p) { if (v >= p.size() || seen[v]) return false; seen[v] = 1; }
        return true; };
    auto isUnitLowerBounded = [](const Matrix& L) {        // 1 on diagonal, 0 above, |entry| <= 1 below
        for (size_t i = 0; i < L.size(); ++i) for (size_t j = 0; j < L.size(); ++j) {
            if (j > i && L[i][j] != 0) return false;
            if (j == i && L[i][j] != 1) return false;
            if (j < i && fabs(L[i][j]) > 1 + 1e-12) return false;
        } return true; };
    auto isUpperTriangular = [](const Matrix& U) {
        for (size_t i = 0; i < U.size(); ++i) for (size_t j = 0; j < i; ++j) if (U[i][j] != 0) return false;
        return true; };
    auto productOfDiagonal = [](const Matrix& U) { double p = 1; for (size_t i = 0; i < U.size(); ++i) p *= U[i][i]; return p; };

    // one call that runs every structural property on a matrix
    auto luChecks = [&](const string& name, const Matrix& M, double eps = 1e-10) {
        LUResult f = luDecompose(M);
        check("lu " + name + ": P*A = L*U", close(permuteRows(M, f.perm), multiplyMatrices(f.L, f.U), eps));
        check("lu " + name + ": L is unit lower triangular, |entries| <= 1", isUnitLowerBounded(f.L));
        check("lu " + name + ": U is upper triangular", isUpperTriangular(f.U));
        check("lu " + name + ": perm is a permutation of 0..n-1", isPermutation(f.perm) && f.perm.size() == M.size());
        check("lu " + name + ": sign = parity of perm", f.sign == parityOf(f.perm));
        check("lu " + name + ": L, U are n x n", rowsOf(f.L) == M.size() && colsOf(f.L) == M.size() &&
                                                  rowsOf(f.U) == M.size() && colsOf(f.U) == M.size());
        bool finite = true;
        for (const Matrix* X : {&f.L, &f.U}) for (const auto& r : *X) for (double x : r) if (!isfinite(x)) finite = false;
        check("lu " + name + ": no NaN or inf in L or U", finite);
    };

    // exact expected values (scipy.linalg.lu reference, converted to PA = LU)
    {
        LUResult f = luDecompose(Matrix{{5}});
        check("lu 1x1: L=1, U=5, perm={0}, sign=+1",
              close(f.L, {{1}}) && close(f.U, {{5}}) && f.perm == vector<size_t>{0} && f.sign == 1);
        LUResult z = luDecompose(Matrix{{0}});
        check("lu 1x1 zero: factors without throwing (L=1, U=0)", close(z.L, {{1}}) && close(z.U, {{0}}));
    }
    {
        LUResult f = luDecompose(A2);       // {{1,2},{3,4}}: pivot 3 forces a swap
        check("lu 2x2: perm = {1,0}, sign = -1", f.perm == vector<size_t>{1,0} && f.sign == -1);
        check("lu 2x2: L = {{1,0},{1/3,1}}", close(f.L, {{1,0},{1.0/3,1}}, 1e-12));
        check("lu 2x2: U = {{3,4},{0,2/3}}", close(f.U, {{3,4},{0,2.0/3}}, 1e-12));
    }
    {
        LUResult f = luDecompose(Matrix{{2,1,1},{4,3,3},{8,7,9}});
        check("lu 3x3: perm = {2,0,1}, sign = +1", f.perm == vector<size_t>{2,0,1} && f.sign == 1);
        check("lu 3x3: L matches scipy", close(f.L, {{1,0,0},{0.25,1,0},{0.5,2.0/3,1}}, 1e-12));
        check("lu 3x3: U matches scipy", close(f.U, {{8,7,9},{0,-0.75,-1.25},{0,0,-2.0/3}}, 1e-12));
    }
    {
        LUResult f = luDecompose(A4);
        check("lu 4x4: perm = {1,2,3,0}, sign = -1", f.perm == vector<size_t>{1,2,3,0} && f.sign == -1);
        check("lu 4x4: U diagonal matches scipy",
              fabs(f.U[0][0]-5) < 1e-12 && fabs(f.U[1][1]-3.6) < 1e-12 && fabs(f.U[2][2]+7.0/3) < 1e-12 && fabs(f.U[3][3]-12.0/7) < 1e-12);
        check("lu 4x4: L[2][1] = -13/18 (scipy)", fabs(f.L[2][1] - (-13.0/18)) < 1e-12);
        check("lu 4x4: determinant = sign * product of U diagonal = 72", fabs(f.sign * productOfDiagonal(f.U) - 72) < 1e-9);
    }
    // pivoting behaviour
    {
        LUResult f = luDecompose(Matrix{{0,1},{1,0}});     // zero in the top-left corner: no-pivot LU would divide by 0
        check("lu zero top-left pivot: swaps instead of dividing by zero", f.perm == vector<size_t>{1,0} && f.sign == -1);
        check("lu zero top-left pivot: L = U = identity", close(f.L, identity(2)) && close(f.U, identity(2)));
        bool allFinite = true; for (auto& r : f.L) for (double x : r) if (!isfinite(x)) allFinite = false;
        for (auto& r : f.U) for (double x : r) if (!isfinite(x)) allFinite = false;
        check("lu zero top-left pivot: no NaN or inf anywhere", allFinite);
    }
    {
        LUResult f = luDecompose(Matrix{{1,2},{1,3}});     // tie in column 0: must NOT swap
        check("lu tie in pivot column keeps original order", f.perm == vector<size_t>{0,1} && f.sign == 1);
    }
    {
        LUResult f = luDecompose(UT);                      // already upper triangular
        check("lu upper triangular input: no swaps, L = I, U = input",
              f.perm == vector<size_t>{0,1,2} && f.sign == 1 && close(f.L, identity(3)) && close(f.U, UT));
        LUResult g = luDecompose(identity(4));
        check("lu identity(4): L = U = I, no swaps", close(g.L, identity(4)) && close(g.U, identity(4)) && g.sign == 1);
    }
    {   // the classic instability: tiny pivot. Without pivoting the factors are garbage.
        Matrix T = {{1e-20,1},{1,1}};
        LUResult f = luDecompose(T);
        check("lu tiny pivot 1e-20: swaps rows (perm = {1,0})", f.perm == vector<size_t>{1,0});
        check("lu tiny pivot 1e-20: |L| entries stay <= 1", isUnitLowerBounded(f.L));
        check("lu tiny pivot 1e-20: P*A = L*U", close(permuteRows(T, f.perm), multiplyMatrices(f.L, f.U), 1e-12));
    }
    // property checks on a spread of matrices (each runs the 6 structural checks)
    luChecks("1x1", Matrix{{5}});
    luChecks("2x2", A2);
    luChecks("3x3 (scipy example)", Matrix{{2,1,1},{4,3,3},{8,7,9}});
    luChecks("4x4", A4);
    luChecks("identity(5)", identity(5));
    luChecks("upper triangular", UT);
    luChecks("needs two swaps", Matrix{{0,2,1},{0,0,3},{4,0,0}});
    luChecks("negative entries", Matrix{{-4,3,-2},{1,-5,6},{7,-8,-9}});
    luChecks("tiny pivot", Matrix{{1e-20,1},{1,1}});
    // singular matrices still factor (rank detection needs this); zero on U's diagonal signals it
    luChecks("singular {{1,2},{2,4}}", Matrix{{1,2},{2,4}});
    luChecks("singular 1..9", Matrix{{1,2,3},{4,5,6},{7,8,9}});
    luChecks("zero 3x3", Matrix{{0,0,0},{0,0,0},{0,0,0}});
    luChecks("zero column", Matrix{{1,0,2},{3,0,4},{5,0,6}});
    {
        LUResult f = luDecompose(Matrix{{1,2},{2,4}});
        check("lu singular 2x2: U[1][1] is (numerically) zero", fabs(f.U[1][1]) < 1e-12);
        LUResult g = luDecompose(Matrix{{0,0},{0,0}});
        check("lu zero matrix: L = I, U = 0, no NaN", close(g.L, identity(2)) && close(g.U, {{0,0},{0,0}}));
        LUResult h = luDecompose(Matrix{{1,2,3},{4,5,6},{7,8,9}});
        check("lu singular 1..9: det via LU is ~0", fabs(h.sign * productOfDiagonal(h.U)) < 1e-12);
    }
    // determinant via LU agrees with the independently verified cofactor determinant
    {
        vector<pair<string, Matrix>> ms = {{"A2", A2}, {"3x3", S3}, {"UT", UT}, {"A4", A4}, {"M3", M3}, {"swap", S3swap}};
        for (auto& nm : ms) {
            LUResult f = luDecompose(nm.second);
            check("lu det via LU = detMatrix: " + nm.first,
                  fabs(f.sign * productOfDiagonal(f.U) - detMatrix(nm.second)) < 1e-9);
        }
    }
    // bigger, deterministic pseudo-random matrices
    {
        unsigned s = 12345;
        auto rnd = [&]() { s = s * 1103515245u + 12345u; return ((s >> 16) & 0x7fff) / 16384.0 - 1.0; };
        for (int n : {6, 8, 12}) {
            Matrix M(n, vector<double>(n));
            for (auto& r : M) for (double& x : r) x = rnd();
            luChecks("random " + to_string(n) + "x" + to_string(n), M);
            if (n <= 8) {
                LUResult f = luDecompose(M);
                check("lu random " + to_string(n) + "x" + to_string(n) + ": det via LU = detMatrix",
                      fabs(f.sign * productOfDiagonal(f.U) - detMatrix(M)) < 1e-9);
            }
        }
    }
    // scale: multiplying A by c must not change the pivoting; U scales, L does not
    for (double c : {1e-7, 1e7}) {
        LUResult f = luDecompose(A4), g = luDecompose(scalarMultiply(c, A4));
        check("lu scaled by " + to_string(c) + ": same perm", f.perm == g.perm && f.sign == g.sign);
        check("lu scaled by " + to_string(c) + ": same L", close(f.L, g.L, 1e-9));
        check("lu scaled by " + to_string(c) + ": U scales by c", close(scalarMultiply(c, f.U), g.U, 1e-9 * c));
    }
    // invalid input and robustness
    check("lu non-square 2x3 throws invalid_argument", throwsInvalid([&]{ luDecompose(R); }));
    check("lu non-square 3x2 throws invalid_argument", throwsInvalid([&]{ luDecompose(C); }));
    check("lu empty (no rows) throws invalid_argument", throwsInvalid([&]{ luDecompose(E0); }));
    check("lu empty (no cols) throws invalid_argument", throwsInvalid([&]{ luDecompose(E1); }));
    check("lu does not modify its input", close(A4, {{1,2,3,4},{5,6,7,8},{2,6,4,8},{3,1,1,2}}) &&
          (luDecompose(A4), close(A4, {{1,2,3,4},{5,6,7,8},{2,6,4,8},{3,1,1,2}})));
    check("lu NaN input: returns without crashing or hanging", !throwsInvalid([&]{ luDecompose(Matrix{{NAN,1},{1,1}}); }));
    check("lu inf input: returns without crashing or hanging", !throwsInvalid([&]{ luDecompose(Matrix{{INFINITY,1},{1,1}}); }));
    check("lu result is reusable: P*A = L*U reconstructs A via inverse permutation", [&]{
        LUResult f = luDecompose(A4); Matrix LU = multiplyMatrices(f.L, f.U); Matrix back(4);
        for (size_t i = 0; i < 4; ++i) back[f.perm[i]] = LU[i];
        return close(back, A4, 1e-10); }());

    // ---- qrDecompose: A = Q*R, Householder, full QR, diag(R) >= 0 ----
    auto isUpperTrapezoidal = [](const Matrix& X) {          // exact zeros strictly below the diagonal
        for (size_t i = 0; i < X.size(); ++i) for (size_t j = 0; j < X[i].size() && j < i; ++j) if (X[i][j] != 0) return false;
        return true; };
    auto diagNonNegative = [](const Matrix& X) {
        for (size_t i = 0; i < X.size() && i < X[0].size(); ++i) if (!(X[i][i] >= 0)) return false;
        return true; };
    auto firstCols = [](const Matrix& X, size_t k) {
        Matrix r(X.size(), vector<double>(k)); for (size_t i = 0; i < X.size(); ++i) for (size_t j = 0; j < k; ++j) r[i][j] = X[i][j]; return r; };
    auto allFinite = [](const Matrix& X) { for (auto& r : X) for (double x : r) if (!isfinite(x)) return false; return true; };
    auto maxAbs = [](const Matrix& X) { double s = 0; for (auto& r : X) for (double x : r) s = max(s, fabs(x)); return s; };

    auto qrChecks = [&](const string& name, const Matrix& M, double eps = 1e-12) {
        QRResult f = qrDecompose(M);
        size_t m = M.size(), n = M[0].size();
        double scale = max(1.0, maxAbs(M));
        check("qr " + name + ": Q is m x m and R is m x n", rowsOf(f.Q) == m && colsOf(f.Q) == m && rowsOf(f.R) == m && colsOf(f.R) == n);
        check("qr " + name + ": Q*R = A", close(multiplyMatrices(f.Q, f.R), M, eps * scale));
        check("qr " + name + ": Q^T*Q = I", close(multiplyMatrices(transposeMatrix(f.Q), f.Q), identity((int)m), eps));
        check("qr " + name + ": Q*Q^T = I", close(multiplyMatrices(f.Q, transposeMatrix(f.Q)), identity((int)m), eps));
        check("qr " + name + ": R is upper triangular (exact zeros)", isUpperTrapezoidal(f.R));
        check("qr " + name + ": diagonal of R is >= 0", diagNonNegative(f.R));
        check("qr " + name + ": no NaN or inf in Q or R", allFinite(f.Q) && allFinite(f.R));
    };

    // reference values from numpy (np.linalg.qr, signs normalised so diag(R) >= 0)
    {
        QRResult f = qrDecompose(A2);                       // {{1,2},{3,4}}
        check("qr 2x2: Q matches numpy", close(f.Q, {{0.316227766016838, 0.9486832980505138},{0.9486832980505138, -0.31622776601683805}}, 1e-12));
        check("qr 2x2: R matches numpy", close(f.R, {{3.1622776601683795, 4.427188724235731},{0, 0.6324555320336753}}, 1e-12));
    }
    {
        QRResult f = qrDecompose(Matrix{{2,1,1},{4,3,3},{8,7,9}});
        check("qr 3x3: Q matches numpy", close(f.Q, {{0.21821789023599236,-0.8164965809277264,0.5345224838248482},
                                                      {0.4364357804719847,-0.4082482904638627,-0.8017837257372735},
                                                      {0.8728715609439694,0.4082482904638631,0.2672612419124247}}, 1e-12));
        check("qr 3x3: R matches numpy", close(f.R, {{9.165151389911681,7.6376261582597325,9.38336928014767},
                                                      {0,0.8164965809277266,1.6329931618554543},
                                                      {0,0,0.5345224838248489}}, 1e-12));
    }
    {
        QRResult f = qrDecompose(C);                        // tall 3x2: only the first 2 columns of Q are fixed by the convention
        check("qr tall 3x2: first 2 columns of Q match numpy",
              close(firstCols(f.Q, 2), {{0.16903085094570325,0.8970852271450607},{0.50709255283711,0.27602622373694136},{0.8451542547285166,-0.345032779671177}}, 1e-12));
        check("qr tall 3x2: R matches numpy (last row is zero)",
              close(f.R, {{5.916079783099616,7.4373574416109465},{0,0.8280786712108248},{0,0}}, 1e-12));
        check("qr tall 3x2: prod diag(R) = sqrt(det(A^T A)) = sqrt(24)",
              fabs(f.R[0][0] * f.R[1][1] - sqrt(detMatrix(multiplyMatrices(transposeMatrix(C), C)))) < 1e-9);
    }
    {
        QRResult f = qrDecompose(R);                        // wide 2x3
        check("qr wide 2x3: Q matches numpy", close(f.Q, {{0.24253562503633308,0.970142500145332},{0.970142500145332,-0.24253562503633289}}, 1e-12));
        check("qr wide 2x3: R matches numpy", close(f.R, {{4.123105625617661,5.335783750799326,6.5484618759809905},{0,0.7276068751089995,1.455213750217998}}, 1e-12));
    }
    // exact small cases and sign convention
    {
        QRResult a = qrDecompose(Matrix{{5}}), b = qrDecompose(Matrix{{-5}}), z = qrDecompose(Matrix{{0}});
        check("qr 1x1 positive: Q=1, R=5", close(a.Q, {{1}}) && close(a.R, {{5}}));
        check("qr 1x1 negative: Q=-1, R=5 (diagonal made non-negative)", close(b.Q, {{-1}}) && close(b.R, {{5}}));
        check("qr 1x1 zero: Q=1, R=0", close(z.Q, {{1}}) && close(z.R, {{0}}));
        QRResult r = qrDecompose(Matrix{{-1,2,3}});
        check("qr row vector {{-1,2,3}}: Q=-1, R={{1,-2,-3}}", close(r.Q, {{-1}}) && close(r.R, {{1,-2,-3}}));
        QRResult c = qrDecompose(Matrix{{3},{0},{4},{0}});
        check("qr column vector: R = norm = 5, Q's first column = x/5", close(c.R, {{5},{0},{0},{0}}) && close(firstCols(c.Q, 1), {{0.6},{0},{0.8},{0}}));
        QRResult n = qrDecompose(Matrix{{-2,1},{0,3}});
        check("qr negative diagonal entry is flipped into Q", close(n.Q, {{-1,0},{0,1}}) && close(n.R, {{2,-1},{0,3}}));
        QRResult u = qrDecompose(UT);
        check("qr upper triangular with positive diagonal: Q = I, R = A", close(u.Q, identity(3)) && close(u.R, UT));
        QRResult i4 = qrDecompose(identity(4));
        check("qr identity(4): Q = I, R = I", close(i4.Q, identity(4)) && close(i4.R, identity(4)));
        QRResult zz = qrDecompose(Matrix{{0,0,0},{0,0,0},{0,0,0}});
        check("qr zero 3x3: Q = I, R = 0, no NaN", close(zz.Q, identity(3)) && close(zz.R, {{0,0,0},{0,0,0},{0,0,0}}));
    }
    // properties on a spread of shapes
    qrChecks("1x1", Matrix{{7}});
    qrChecks("2x2", A2);
    qrChecks("3x3", Matrix{{2,1,1},{4,3,3},{8,7,9}});
    qrChecks("4x4", A4);
    qrChecks("identity(5)", identity(5));
    qrChecks("tall 3x2", C);
    qrChecks("wide 2x3", R);
    qrChecks("negative entries", Matrix{{-4,3,-2},{1,-5,6},{7,-8,-9}});
    qrChecks("zero 3x3", Matrix{{0,0,0},{0,0,0},{0,0,0}});
    qrChecks("zero column in the middle", Matrix{{1,0,2},{3,0,4},{5,0,6}});
    qrChecks("zero first column", Matrix{{0,1},{0,2},{0,3}});
    qrChecks("rank-deficient 1..9", Matrix{{1,2,3},{4,5,6},{7,8,9}});
    qrChecks("duplicate columns", Matrix{{1,1},{2,2},{3,3}});
    qrChecks("tall single column", Matrix{{1},{2},{3},{4},{5}});
    {   // cancellation trap: first column almost along e1, so the "wrong" reflector sign loses all precision
        Matrix T = {{1,1},{1e-9,2},{0,3}};
        qrChecks("nearly-axis-aligned column (cancellation trap)", T);
        QRResult f = qrDecompose(T);
        check("qr cancellation trap: R[0][0] = sqrt(1+1e-18) = 1", fabs(f.R[0][0] - 1) < 1e-12);
        check("qr cancellation trap: Q's first column = (1, 1e-9, 0)", close(firstCols(f.Q, 1), {{1},{1e-9},{0}}, 1e-12));
    }
    {   // Hilbert 8x8: condition number ~1e10. Householder must keep Q orthogonal anyway.
        Matrix H(8, vector<double>(8));
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) H[i][j] = 1.0 / (i + j + 1);
        qrChecks("Hilbert 8x8 (ill-conditioned)", H);
    }
    // square matrices: |det A| = product of diag(R), cross-checked against the verified determinant
    {
        vector<pair<string, Matrix>> ms = {{"A2", A2}, {"3x3", S3}, {"UT", UT}, {"A4", A4}, {"M3", M3}, {"negative", Matrix{{-4,3,-2},{1,-5,6},{7,-8,-9}}}};
        for (auto& nm : ms) {
            QRResult f = qrDecompose(nm.second); double p = 1; for (size_t i = 0; i < f.R.size(); ++i) p *= f.R[i][i];
            double d = fabs(detMatrix(nm.second));
            check("qr |det| = product of diag(R): " + nm.first, fabs(p - d) < 1e-9 * max(1.0, d));
        }
        QRResult s = qrDecompose(Matrix{{1,2,3},{4,5,6},{7,8,9}});
        check("qr singular 1..9: last diagonal of R is ~0", fabs(s.R[2][2]) < 1e-12);
    }
    // random matrices of several shapes
    {
        unsigned s = 987654321u;
        auto rnd = [&]() { s = s * 1103515245u + 12345u; return ((s >> 16) & 0x7fff) / 16384.0 - 1.0; };
        for (auto dims : vector<pair<int,int>>{{5,5},{8,8},{12,12},{10,4},{3,7},{2,9},{15,1},{1,6}}) {
            Matrix M(dims.first, vector<double>(dims.second));
            for (auto& r : M) for (double& x : r) x = rnd();
            qrChecks("random " + to_string(dims.first) + "x" + to_string(dims.second), M);
        }
        Matrix T(10, vector<double>(4)); for (auto& r : T) for (double& x : r) x = rnd();
        QRResult f = qrDecompose(T); double p = 1; for (int i = 0; i < 4; ++i) p *= f.R[i][i];
        check("qr random tall 10x4: prod diag(R) = sqrt(det(A^T A))",
              fabs(p - sqrt(detMatrix(multiplyMatrices(transposeMatrix(T), T)))) < 1e-9);
    }
    // scale: Q must not change, R scales by c; huge and tiny scales must not overflow or underflow to zero
    for (double c : {1e-200, 1e-7, 1e7, 1e200}) {
        QRResult f = qrDecompose(A4), g = qrDecompose(scalarMultiply(c, A4));
        check("qr scaled by " + to_string(c) + ": no NaN/inf", allFinite(g.Q) && allFinite(g.R));
        check("qr scaled by " + to_string(c) + ": same Q", close(f.Q, g.Q, 1e-9));
        check("qr scaled by " + to_string(c) + ": R scales by c", close(scalarMultiply(1.0 / c, g.R), f.R, 1e-9));
    }
    // invalid input and robustness
    check("qr empty (no rows) throws invalid_argument", throwsInvalid([&]{ qrDecompose(E0); }));
    check("qr empty (no cols) throws invalid_argument", throwsInvalid([&]{ qrDecompose(E1); }));
    check("qr ragged rows throw invalid_argument (short last row)", throwsInvalid([&]{ qrDecompose(Matrix{{1,2},{3}}); }));
    check("qr ragged rows throw invalid_argument (short middle row)", throwsInvalid([&]{ qrDecompose(Matrix{{1,2,3},{4,5},{6,7,8}}); }));
    check("qr does not modify its input", (qrDecompose(A4), close(A4, {{1,2,3,4},{5,6,7,8},{2,6,4,8},{3,1,1,2}})));
    check("qr NaN input: returns without crashing or hanging", !throwsAny([&]{ qrDecompose(Matrix{{NAN,1},{1,1}}); }));
    check("qr inf input: returns without crashing or hanging", !throwsAny([&]{ qrDecompose(Matrix{{INFINITY,1},{1,1}}); }));

    cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
