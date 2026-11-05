#ifndef part1_arithmetic_hpp
#define part1_arithmetic_hpp

#include <vector>

namespace part1 {

using Array = std::vector<double>;

// Element-wise addition
Array add(const Array& a, const Array& b);

// Element-wise subtraction
Array subtract(const Array& a, const Array& b);

// Element-wise multiplication
Array multiply(const Array& a, const Array& b);

// Element-wise division
Array divide(const Array& a, const Array& b);

} // namespace part1

#endif // part1_arithmetic_hpp
