#include <ndarray.hpp>

#include <stdexcept>
#include <limits>

NDArray::NDArray(
    const std::vector<std::size_t>& shape,
    const std::vector<double>& values)
    : data(values), dimensions(shape)
{
    if (dimensions.empty())
    {
        throw std::invalid_argument(
            "Array must have at least one dimension"
        );
    }

    if (calculate_size(dimensions) != data.size())
    {
        throw std::invalid_argument(
            "Shape does not match the number of values"
        );
    }
}

std::size_t NDArray::calculate_size(
    const std::vector<std::size_t>& shape) const
{
    if (shape.empty())
    {
        throw std::invalid_argument(
            "Shape cannot be empty"
        );
    }

    std::size_t total = 1;

    for (std::size_t dimension : shape)
    {
        if (dimension == 0)
        {
            throw std::invalid_argument(
                "Dimensions must be greater than zero"
            );
        }

        if (total >
            std::numeric_limits<std::size_t>::max()
                / dimension)
        {
            throw std::overflow_error(
                "Array size is too large"
            );
        }

        total *= dimension;
    }

    return total;
}

std::size_t NDArray::calculate_index(
    const std::vector<std::size_t>& indices) const
{
    if (indices.size() != dimensions.size())
    {
        throw std::invalid_argument(
            "Incorrect number of indices"
        );
    }

    std::size_t index = 0;

    for (std::size_t i = 0; i < dimensions.size(); i++)
    {
        if (indices[i] >= dimensions[i])
        {
            throw std::out_of_range(
                "Array index is out of range"
            );
        }

        index = index * dimensions[i] + indices[i];
    }

    return index;
}

std::size_t NDArray::size() const
{
    return data.size();
}

std::size_t NDArray::ndim() const
{
    return dimensions.size();
}

std::vector<std::size_t> NDArray::shape() const
{
    return dimensions;
}

double& NDArray::at(
    const std::vector<std::size_t>& indices)
{
    return data[calculate_index(indices)];
}

const double& NDArray::at(
    const std::vector<std::size_t>& indices) const
{
    return data[calculate_index(indices)];
}

const std::vector<double>& NDArray::values() const
{
    return data;
}

void NDArray::reshape(
    const std::vector<std::size_t>& new_shape)
{
    if (calculate_size(new_shape) != data.size())
    {
        throw std::invalid_argument(
            "New shape must contain the same number of elements"
        );
    }

    dimensions = new_shape;
}
