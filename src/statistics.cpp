#include "statistics.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace statistics
{
    double mean(const std::vector<double>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Data cannot be empty");
        }

        double sum = 0;

        for (double value : data)
        {
            sum += value;
        }

        return sum / data.size();
    }


    double median(const std::vector<double>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Data cannot be empty");
        }

        std::vector<double> sorted_data = data;

        std::sort(sorted_data.begin(), sorted_data.end());

        int n = sorted_data.size();

        if (n % 2 == 0)
        {
            return (sorted_data[n / 2 - 1] +
                    sorted_data[n / 2]) / 2.0;
        }

        return sorted_data[n / 2];
    }


    double minimum(const std::vector<double>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Data cannot be empty");
        }

        return *std::min_element(
            data.begin(),
            data.end()
        );
    }


    double maximum(const std::vector<double>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Data cannot be empty");
        }

        return *std::max_element(
            data.begin(),
            data.end()
        );
    }


    double standard_deviation(
        const std::vector<double>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Data cannot be empty");
        }

        double avg = mean(data);
        double sum = 0;

        for (double value : data)
        {
            sum += (value - avg) * (value - avg);
        }

        return std::sqrt(sum / data.size());
    }


    double correlation(
        const std::vector<double>& x,
        const std::vector<double>& y)
    {
        if (x.empty() || y.empty())
        {
            throw std::invalid_argument(
                "Data cannot be empty"
            );
        }

        if (x.size() != y.size())
        {
            throw std::invalid_argument(
                "Datasets must have the same size"
            );
        }

        double mean_x = mean(x);
        double mean_y = mean(y);

        double numerator = 0;
        double denominator_x = 0;
        double denominator_y = 0;

        for (std::size_t i = 0; i < x.size(); ++i)
        {
            numerator +=
                (x[i] - mean_x) *
                (y[i] - mean_y);

            denominator_x +=
                (x[i] - mean_x) *
                (x[i] - mean_x);

            denominator_y +=
                (y[i] - mean_y) *
                (y[i] - mean_y);
        }

        const double denominator = std::sqrt(denominator_x * denominator_y);
        if (denominator == 0.0) {
            throw std::domain_error("Correlation is undefined when either variable has zero variance");
        }
        return numerator / denominator;
    }


    int count(const std::vector<double>& data)
    {
        return data.size();
    }


    double quantile(
        const std::vector<double>& data,
        double q)
    {
        if (data.empty())
        {
            throw std::invalid_argument(
                "Data cannot be empty"
            );
        }

        if (q < 0 || q > 1)
        {
            throw std::invalid_argument(
                "Quantile must be between 0 and 1"
            );
        }

        std::vector<double> sorted_data = data;

        std::sort(
            sorted_data.begin(),
            sorted_data.end()
        );

        double position =
            q * (sorted_data.size() - 1);

        int lower = std::floor(position);
        int upper = std::ceil(position);

        if (lower == upper)
        {
            return sorted_data[lower];
        }

        return sorted_data[lower] +
               (position - lower) *
               (sorted_data[upper] -
                sorted_data[lower]);
    }


    double percentile(
        const std::vector<double>& data,
        double p)
    {
        if (p < 0 || p > 100)
        {
            throw std::invalid_argument(
                "Percentile must be between 0 and 100"
            );
        }

        return quantile(data, p / 100.0);
    }
}
