#ifndef VEC3_H
#define VEC3_H
#include <cmath>
class Vec3
{
private:
    double _x;
    double _y;
    double _z;

public:
    Vec3(double newX, double newY, double newZ);
    Vec3(const Vec3& newVec3);
    Vec3();
    ~Vec3();
    double getX() const;
    double getY() const;
    double getZ() const;
    void setX(double newX);
    void setY(double newY);
    void setZ(double newZ);
    Vec3 normalize();
};
#endif
