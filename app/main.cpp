#include "aggregation.hpp"
#include "array_creation.hpp"
#include "array_utils.hpp"
#include "broadcasting.hpp"
#include "indexing.hpp"
#include "matrix.hpp"
#include "part1_arithmetic.hpp"
#include "part2_functions.hpp"
#include "statistics.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::string prompt_line(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Input ended.");
    return line;
}
double read_double(const std::string& prompt) {
    while (true) {
        std::string line = prompt_line(prompt);
        std::istringstream in(line);
        double value; char extra;
        if (in >> value && !(in >> extra)) return value;
        std::cout << "Invalid input. Enter one numeric value, e.g. 3.5.\n";
    }
}
std::size_t read_size(const std::string& prompt, bool allow_zero = false) {
    while (true) {
        std::string line = prompt_line(prompt);
        std::istringstream in(line);
        long long value; char extra;
        if (in >> value && !(in >> extra) && value >= (allow_zero ? 0 : 1))
            return static_cast<std::size_t>(value);
        std::cout << "Enter a whole number " << (allow_zero ? ">= 0" : "> 0") << ".\n";
    }
}
int read_int(const std::string& prompt) {
    while (true) {
        std::string line = prompt_line(prompt);
        std::istringstream in(line);
        int value; char extra;
        if (in >> value && !(in >> extra)) return value;
        std::cout << "Enter a valid integer.\n";
    }
}
std::vector<double> read_vector(const std::string& label) {
    while (true) {
        std::cout << label << "\nEnter the number of elements, then the values separated by spaces or commas.\n";
        std::size_t n = read_size("Number of elements: ");
        std::string line = prompt_line("Values: ");
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream in(line);
        std::vector<double> v(n);
        bool ok = true;
        for (double& x : v) if (!(in >> x)) { ok = false; break; }
        std::string extra;
        if (ok && !(in >> extra)) return v;
        std::cout << "Expected exactly " << n << " numeric values. Try again.\n";
    }
}
std::vector<std::size_t> read_shape() {
    std::size_t rank = read_size("Number of dimensions (1=vector, 2=matrix, 3=3D array): ");
    if (rank > 8) throw std::invalid_argument("This interactive tool supports at most 8 dimensions.");
    std::vector<std::size_t> shape(rank);
    for (std::size_t i = 0; i < rank; ++i)
        shape[i] = read_size("Size of dimension " + std::to_string(i + 1) + ": ");
    return shape;
}
std::size_t element_count(const std::vector<std::size_t>& shape) {
    std::size_t n = 1;
    for (auto d : shape) {
        if (d && n > std::numeric_limits<std::size_t>::max() / d)
            throw std::overflow_error("Array is too large.");
        n *= d;
    }
    return n;
}
NDArray read_ndarray(const std::string& label) {
    std::cout << "\n" << label << "\n";
    auto shape = read_shape();
    std::size_t n = element_count(shape);
    std::cout << "Enter exactly " << n << " numeric element(s), in row-major order.\n";
    while (true) {
        std::string line = prompt_line("Array values (space- or comma-separated): ");
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream in(line);
        std::vector<double> values(n);
        bool ok = true;
        for (double& value : values) if (!(in >> value)) { ok = false; break; }
        std::string extra;
        if (ok && !(in >> extra)) return NDArray(shape, values);
        std::cout << "Expected exactly " << n << " numeric values. Try again.\n";
    }
}
void print_shape(const std::vector<std::size_t>& shape) {
    std::cout << '(';
    for (std::size_t i = 0; i < shape.size(); ++i) {
        if (i) std::cout << ", ";
        std::cout << shape[i];
    }
    std::cout << ')';
}
void print_array(const NDArray& a) {
    std::cout << "Shape: "; print_shape(a.shape());
    std::cout << "\nValues (row-major): ";
    for (double v : a.values()) std::cout << v << ' ';
    std::cout << '\n';
}
Matrix read_matrix(const std::string& label) {
    std::cout << "\n" << label << "\n";
    std::size_t rows = read_size("Number of rows: ");
    std::size_t cols = read_size("Number of columns: ");
    Matrix result(rows, std::vector<double>(cols));
    for (std::size_t r = 0; r < rows; ++r) {
        while (true) {
            std::string line = prompt_line("Row " + std::to_string(r + 1) +
                ": enter exactly " + std::to_string(cols) + " values: ");
            std::replace(line.begin(), line.end(), ',', ' ');
            std::istringstream in(line);
            bool ok = true;
            for (double& x : result[r]) if (!(in >> x)) { ok = false; break; }
            std::string extra;
            if (ok && !(in >> extra)) break;
            std::cout << "Invalid row. Please enter exactly " << cols << " numbers.\n";
        }
    }
    return result;
}
void print_vector(const std::vector<double>& values) {
    for (double v : values) std::cout << v << ' ';
    std::cout << '\n';
}
void statistics_menu() {
    auto x = read_vector("STATISTICS: enter a single 1D dataset.");
    std::cout << "Mean: " << statistics::mean(x)
              << "\nMedian: " << statistics::median(x)
              << "\nMinimum: " << statistics::minimum(x)
              << "\nMaximum: " << statistics::maximum(x)
              << "\nStandard deviation: " << statistics::standard_deviation(x)
              << "\nCount: " << statistics::count(x)
              << "\n25th percentile: " << statistics::percentile(x, 25)
              << "\n75th percentile: " << statistics::percentile(x, 75) << '\n';
    std::string answer = prompt_line("Calculate correlation with a second 1D dataset? (y/n): ");
    if (answer == "y" || answer == "Y") {
        auto y = read_vector("SECOND DATASET: must have the same number of values as the first.");
        std::cout << "Correlation: " << statistics::correlation(x, y) << '\n';
    }
}
void creation_menu() {
    std::cout << "ARRAY CREATION\n";
    auto shape = read_shape();
    std::cout << "1=zeros, 2=ones, 3=constant-filled array: ";
    int mode = read_int("");
    NDArray a = mode == 1 ? array_creation::zeros(shape) :
                mode == 2 ? array_creation::ones(shape) :
                mode == 3 ? array_creation::full(shape, read_double("Constant value for every element: ")) :
                throw std::invalid_argument("Choose 1, 2, or 3.");
    print_array(a);
}
void aggregation_menu() {
    NDArray a = read_ndarray("AGGREGATION: enter the array to reduce.");
    std::cout << "Total sum: " << aggregation::sum(a) << '\n';
    if (a.ndim() < 2) { std::cout << "Axis reductions require at least a 2D array.\n"; return; }
    for (std::size_t axis = 0; axis < a.ndim(); ++axis) {
        std::cout << "Axis " << axis << " sum: ";
        print_array(aggregation::sum(a, axis));
        std::cout << "Axis " << axis << " minimum: ";
        print_array(aggregation::minimum(a, axis));
        std::cout << "Axis " << axis << " maximum: ";
        print_array(aggregation::maximum(a, axis));
    }
}
void indexing_menu() {
    NDArray a = read_ndarray("INDEXING: enter the array to inspect.");
    std::vector<std::size_t> index(a.ndim());
    std::cout << "Indices are zero-based (first position is 0).\n";
    for (std::size_t i = 0; i < index.size(); ++i)
        index[i] = read_size("Index for dimension " + std::to_string(i + 1) + ": ", true);
    std::cout << "Element at requested index: " << indexing::get(a, index) << '\n';
    std::string answer = prompt_line("Slice one dimension? (y/n): ");
    if (answer == "y" || answer == "Y") {
        auto axis = read_size("Axis to slice (zero-based): ", true);
        auto start = read_size("Start index (inclusive): ", true);
        auto stop = read_size("Stop index (exclusive): ", true);
        auto step = read_size("Step (positive integer): ");
        print_array(indexing::slice(a, axis, start, stop, step));
    }
}
void load_menu() {
    std::string path = prompt_line("File path [data/input/sample_2d_array.txt]: ");
    if (path.empty()) path = "data/input/sample_2d_array.txt";
    auto a = array_utils::load(path);
    print_array(a);
    std::string out = prompt_line("Output path [data/output/round_trip_array.txt]: ");
    if (out.empty()) out = "data/output/round_trip_array.txt";
    array_utils::save(a, out);
    std::cout << "Saved copy to " << out << '\n';
}
void matrix_menu() {
    Matrix a = read_matrix("MATRIX A");
    Matrix b = read_matrix("MATRIX B");
    std::cout << "\n1=matrix multiplication, 2=addition, 3=subtraction, 4=element-wise product, 5=transpose A\n";
    int op = read_int("Operation: ");
    if (op == 1) displayMatrix(multiplyMatrices(a, b));
    else if (op == 2) displayMatrix(addMatrices(a, b));
    else if (op == 3) displayMatrix(subtractMatrices(a, b));
    else if (op == 4) displayMatrix(hadamard(a, b));
    else if (op == 5) displayMatrix(transposeMatrix(a));
    else throw std::invalid_argument("Unknown matrix operation.");
}
void vector_menu() {
    auto a = read_vector("VECTOR A");
    std::cout << "1=add, 2=subtract, 3=element-wise multiply, 4=element-wise divide\n";
    int op = read_int("Operation: ");
    auto b = read_vector("VECTOR B (must have the same length as Vector A)");
    std::vector<double> result;
    if (op == 1) result = part1::add(a, b);
    else if (op == 2) result = part1::subtract(a, b);
    else if (op == 3) result = part1::multiply(a, b);
    else if (op == 4) result = part1::divide(a, b);
    else throw std::invalid_argument("Unknown vector operation.");
    std::cout << "Result: "; print_vector(result);
}
void broadcast_menu() {
    NDArray a = read_ndarray("BROADCASTING: input array A.");
    NDArray b = read_ndarray("BROADCASTING: input array B.");
    std::cout << "1=add, 2=subtract, 3=element-wise multiply, 4=element-wise divide\n";
    int op = read_int("Broadcast operation: ");
    NDArray result = op == 1 ? shadic::broadcast_add(a, b) :
                     op == 2 ? shadic::broadcast_subtract(a, b) :
                     op == 3 ? shadic::broadcast_multiply(a, b) :
                     op == 4 ? shadic::broadcast_divide(a, b) :
                     throw std::invalid_argument("Choose operation 1-4.");
    std::cout << "Broadcast result: "; print_array(result);
}
void function_finder() {
    struct Entry { const char* name; const char* file; const char* description; };
    const Entry entries[] = {
        {"statistics::mean", "src/statistics.cpp / include/statistics.hpp", "Arithmetic mean"},
        {"statistics::median", "src/statistics.cpp / include/statistics.hpp", "Median"},
        {"statistics::correlation", "src/statistics.cpp / include/statistics.hpp", "Correlation between two datasets"},
        {"array_creation::zeros", "src/array_creation.cpp / include/array_creation.hpp", "Create an array filled with zero"},
        {"array_creation::ones", "src/array_creation.cpp / include/array_creation.hpp", "Create an array filled with one"},
        {"aggregation::sum", "src/aggregation.cpp / include/aggregation.hpp", "Sum all values or reduce along an axis"},
        {"indexing::get", "src/indexing.cpp / include/indexing.hpp", "Read an element using zero-based indices"},
        {"indexing::set", "src/indexing.cpp / include/indexing.hpp", "Change an element"},
        {"indexing::slice", "src/indexing.cpp / include/indexing.hpp", "Slice one axis"},
        {"array_utils::load", "src/array_utils.cpp / include/array_utils.hpp", "Load array from a text file"},
        {"array_utils::save", "src/array_utils.cpp / include/array_utils.hpp", "Save array to a text file"},
        {"multiplyMatrices", "src/matrix.cpp / include/matrix.hpp", "Matrix multiplication"},
        {"transposeMatrix", "src/matrix.cpp / include/matrix.hpp", "Transpose a matrix"},
        {"part1::add", "src/part1_arithmetic.cpp / include/part1_arithmetic.hpp", "Vector element-wise addition"},
        {"part2::power", "src/part2_Mathematical_functions.cpp / include/part2_functions.hpp", "Raise a number to a power"},
        {"shadic::ShapeTracker::broadcast_shapes", "include/broadcasting.hpp", "Calculate the compatible output shape"},
        {"shadic::broadcast_add", "src/broadcasting.cpp / include/broadcasting.hpp", "Add arrays using broadcasting"},
        {"shadic::broadcast_subtract", "src/broadcasting.cpp / include/broadcasting.hpp", "Subtract arrays using broadcasting"},
        {"shadic::broadcast_multiply", "src/broadcasting.cpp / include/broadcasting.hpp", "Multiply arrays using broadcasting"},
        {"shadic::broadcast_divide", "src/broadcasting.cpp / include/broadcasting.hpp", "Divide arrays using broadcasting"}
    };
    std::string query = prompt_line("Function finder: enter a function name, keyword, or file name: ");
    std::transform(query.begin(), query.end(), query.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    bool found = false;
    for (const auto& e : entries) {
        std::string searchable = std::string(e.name) + " " + e.file + " " + e.description;
        std::transform(searchable.begin(), searchable.end(), searchable.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        if (searchable.find(query) != std::string::npos) {
            std::cout << "\nFunction: " << e.name << "\nFile: " << e.file
                      << "\nPurpose: " << e.description << "\n";
            found = true;
        }
    }
    if (!found) std::cout << "No match. Try a shorter keyword such as 'matrix', 'mean', 'broadcast', or 'indexing'.\n";
}
void help_menu() {
    std::cout << "\nFUNCTION FINDER HELP\n"
              << "Choose menu option 10 and type a function, keyword, or source filename.\n"
              << "Examples: 'mean', 'matrix', 'broadcast', 'indexing', 'load'.\n"
              << "The finder searches its built-in index; update function_finder() when adding functions.\n";
}
}

int main() {
    while (true) {
        std::cout << "\n=== NUMERICAL COMPUTING LIBRARY ===\n"
                  << "1. Statistics (1D data)\n"
                  << "2. Create array (vector/matrix/ND array)\n"
                  << "3. Aggregation and axis reductions (enter array)\n"
                  << "4. Indexing and slicing (enter array and indices)\n"
                  << "5. Load and save array data\n"
                  << "6. Matrix operations (enter matrices)\n"
                  << "7. Scalar mathematical functions\n"
                  << "8. Vector arithmetic (enter vectors)\n"
                  << "9. Broadcasting operations (enter arrays)\n"
                  << "10. Function key finder\n"
                  << "11. Function finder help\n"
                  << "0. Exit\n";
        int choice;
        try {
            choice = read_int("Select an operation: ");
            if (choice == 0) break;
            switch (choice) {
                case 1: statistics_menu(); break;
                case 2: creation_menu(); break;
                case 3: aggregation_menu(); break;
                case 4: indexing_menu(); break;
                case 5: load_menu(); break;
                case 6: matrix_menu(); break;
                case 7: {
                    double x = read_double("Enter one scalar numeric value x: ");
                    std::cout << "x^2: " << part2::power(x, 2)
                              << "\nsqrt(abs(x)): " << part2::squareRoot(part2::absoluteValue(x))
                              << "\nabs(x): " << part2::absoluteValue(x)
                              << "\nexp(x): " << part2::exponential(x) << '\n';
                    break;
                }
                case 8: vector_menu(); break;
                case 9: broadcast_menu(); break;
                case 10: function_finder(); break;
                case 11: help_menu(); break;
                default: std::cout << "Unknown choice. Select a listed menu number.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Operation failed: " << e.what() << "\nPlease check the dimensions and values, then try again.\n";
        }
    }
    std::cout << "Goodbye.\n";
    return 0;
}
