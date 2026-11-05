#include "addition.hpp"
#include <random>
#include <stdexcept>

namespace numlib {
namespace {
std::mt19937& generator() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}
}

void set_random_seed(unsigned int seed) {
    generator().seed(seed);
}

std::vector<double> random_uniform(size_t count, double min, double max) {
    if (!(min < max))
        throw std::invalid_argument("random_uniform requires min < max.");
    std::uniform_real_distribution<double> distribution(min, max);
    std::vector<double> result(count);
    for (double& value : result) value = distribution(generator());
    return result;
}

std::vector<int> random_integer(size_t count, int min, int max) {
    if (min > max)
        throw std::invalid_argument("random_integer requires min <= max.");
    std::uniform_int_distribution<int> distribution(min, max);
    std::vector<int> result(count);
    for (int& value : result) value = distribution(generator());
    return result;
}
}
