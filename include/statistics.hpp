#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>

namespace statistics
{
    double mean(const std::vector<double>& data);

    double median(const std::vector<double>& data);

    double minimum(const std::vector<double>& data);

    double maximum(const std::vector<double>& data);

    double standard_deviation(const std::vector<double>& data);

    double correlation(
        const std::vector<double>& x,
        const std::vector<double>& y
    );

    int count(const std::vector<double>& data);

    double quantile(
        const std::vector<double>& data,
        double q
    );

    double percentile(
        const std::vector<double>& data,
        double p
    );
}

#endif
