#ifndef VECTOR2_HPP
#define VECTOR2_HPP

/* Vector2 class
 * Mathematical building block - Contains a 2D real vector */

class Vector2
{
public:
    double x, y;

    double norm() const;
    double normSquared() const;
};

#endif