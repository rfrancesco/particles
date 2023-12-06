#include "Vector2.hpp"
#include <cmath>

double Vector2::normSquared() const
{
    return x * x + y * y;
}

double Vector2::norm() const
{
    return std::sqrt(normSquared());
}