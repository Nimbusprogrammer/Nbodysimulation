#ifndef VEC3_H
#define VEC3_H
#include  <cmath>
struct Vec3
{
    double x, y, z;
    //constructor will initialize the vector
    Vec3(double xval = 0.00 , double yval = 0.00, double zval = 0.00)
        :x(xval), y(yval), z(zval) {}
    Vec3 operator+(const Vec3& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    Vec3 operator-(const Vec3& other) const
    {
        return Vec3(x-other.x, y-other.y, z-other.z);
    }
    Vec3 operator*(double scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    Vec3 operator/(double scalar) const
    {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }
    //modifying THIS Vec3
    Vec3& operator+=(const Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this; // Return my reference to myself( update )
    }
    Vec3& operator-=(const Vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
   Vec3& operator*=(const double scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vec3& operator/=(const double scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    double magSq() const
    {
        return x*x + y*y + z*z;
    }

    // Returns the actual length of the vector
    double mag() const
    {
        return std::sqrt(magSq());
    }
};
#endif // VEC3_H