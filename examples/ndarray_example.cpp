#include <iostream>
#include <ndarray.hpp>

int main()
{
    NDArray array(
        {2, 3},
        {1, 2, 3, 4, 5, 6}
    );

    std::cout << "Number of dimensions: "
              << array.ndim() << '\n';

    std::cout << "Total elements: "
              << array.size() << '\n';

    std::cout << "Shape: ";

    for (std::size_t dimension : array.shape())
    {
        std::cout << dimension << " ";
    }

    std::cout << "\n\nArray values:\n";

    for (std::size_t row = 0; row < 2; row++)
    {
        for (std::size_t col = 0; col < 3; col++)
        {
            std::cout << array.at({row, col}) << " ";
        }

        std::cout << '\n';
    }

    std::cout << "\nElement at (1, 1): "
              << array.at({1, 1}) << '\n';

    array.at({1, 1}) = 50;

    std::cout << "Updated element: "
              << array.at({1, 1}) << '\n';

    array.reshape({3, 2});

    std::cout << "\nNew shape: ";

    for (std::size_t dimension : array.shape())
    {
        std::cout << dimension << " ";
    }

    std::cout << '\n';

    return 0;
}
