#include <deque>
#include <numeric>
#include <exception>

class DataSmoother
{
    std::deque<double> data;
    int maxCapacity{250};

public:
    explicit DataSmoother(int capacity) : maxCapacity{capacity}
    {
        if (capacity <= 0)
            throw std::invalid_argument("Cannot create a DataSmoother with zero or negative capacity!");
    }
    DataSmoother &operator<<(double dataPoint)
    {
        data.push_back(dataPoint);
        while (data.size() > maxCapacity)
            data.pop_front();
        return *this;
    }
    double mean() const { return std::accumulate(data.begin(), data.end(), 0.) / data.size(); };
};