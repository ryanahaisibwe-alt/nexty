#ifndef NDARRAY_HPP
#define NDARRAY_HPP

#include <cstddef>
#include <vector>

class NDArray
{
private:
    std::vector<double> data;
    std::vector<std::size_t> dimensions;

    std::size_t calculate_size(
        const std::vector<std::size_t>& shape
    ) const;

    std::size_t calculate_index(
        const std::vector<std::size_t>& indices
    ) const;

public:
    NDArray(
        const std::vector<std::size_t>& shape,
        const std::vector<double>& values
    );

    std::size_t size() const;

    std::size_t ndim() const;

    std::vector<std::size_t> shape() const;

    double& at(
        const std::vector<std::size_t>& indices
    );

    const double& at(
        const std::vector<std::size_t>& indices
    ) const;

    const std::vector<double>& values() const;

    void reshape(
        const std::vector<std::size_t>& new_shape
    );
};

#endif
