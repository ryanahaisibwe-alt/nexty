#include <cassert>
#include <stdexcept>
#include <ndarray.hpp>

int main()
{
    NDArray array(
        {2, 3},
        {1, 2, 3, 4, 5, 6}
    );

    assert(array.size() == 6);
    assert(array.ndim() == 2);
    assert(array.shape() ==
           std::vector<std::size_t>({2, 3}));

    assert(array.at({0, 0}) == 1);
    assert(array.at({1, 1}) == 5);
    assert(array.at({1, 2}) == 6);

    array.at({0, 0}) = 10;
    assert(array.at({0, 0}) == 10);

    array.reshape({3, 2});

    assert(array.shape() ==
           std::vector<std::size_t>({3, 2}));

    bool error_detected = false;

    try
    {
        array.reshape({4, 2});
    }
    catch (const std::invalid_argument&)
    {
        error_detected = true;
    }

    assert(error_detected);

    error_detected = false;

    try
    {
        array.at({3, 0});
    }
    catch (const std::out_of_range&)
    {
        error_detected = true;
    }

    assert(error_detected);

    return 0;
}
