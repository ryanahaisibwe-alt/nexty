#include <iostream>
#include <vector>

#include <statistics.hpp>

int main()
{
    std::vector<double> data = {
        10, 20, 30, 40, 50
    };

    std::cout << "Mean: "
              << statistics::mean(data)
              << std::endl;

    std::cout << "Median: "
              << statistics::median(data)
              << std::endl;

    std::cout << "Minimum: "
              << statistics::minimum(data)
              << std::endl;

    std::cout << "Maximum: "
              << statistics::maximum(data)
              << std::endl;

    std::cout << "Standard deviation: "
              << statistics::standard_deviation(data)
              << std::endl;

    std::cout << "Count: "
              << statistics::count(data)
              << std::endl;

    std::cout << "75th percentile: "
              << statistics::percentile(data, 75)
              << std::endl;

    return 0;
}
