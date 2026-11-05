#include "aggregation.hpp"

#include <iostream>

int main()
{
    NDArray array({2, 3}, {1, 2, 3, 4, 5, 6});

    std::cout << "Total sum: "
              << aggregation::sum(array) << '\n';

    NDArray row_sums = aggregation::sum(array, 1);
    NDArray column_sums = aggregation::sum(array, 0);
    NDArray row_minimums = aggregation::minimum(array, 1);
    NDArray row_maximums = aggregation::maximum(array, 1);

    std::cout << "Row sums: ";
    for (double value : row_sums.values())
        std::cout << value << " ";

    std::cout << "\nColumn sums: ";
    for (double value : column_sums.values())
        std::cout << value << " ";

    std::cout << "\nRow minimums: ";
    for (double value : row_minimums.values())
        std::cout << value << " ";

    std::cout << "\nRow maximums: ";
    for (double value : row_maximums.values())
        std::cout << value << " ";

    NDArray second({2, 3}, {10, 20, 30, 40, 50, 60});
    NDArray result = aggregation::add(array, second);

    std::cout << "\nElement-wise addition: ";
    for (double value : result.values())
        std::cout << value << " ";

    std::cout << '\n';

    return 0;
}
