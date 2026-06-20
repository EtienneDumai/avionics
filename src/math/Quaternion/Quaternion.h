#ifndef QUATERNION_H
#define QUATERNION_H
#include "../Vec3/Vec3.h"
#include <cmath>
class Quaternion
{
private:
    double _x;
    double _y;
    double _z;
    double _w;

public:
    Quaternion(double x, double y, double z, double w);
    Quaternion(double angle, Vec3* axis);
    Quaternion(const Quaternion& copy);
    Quaternion();
    ~Quaternion();
    double getX() const;
    double getY() const;
    double getZ() const;
    double getW() const;
    void setX(double newX);
    void setY(double newY);
    void setZ(double newZ);
    void setW(double newW);
    Quaternion multiply(const Quaternion& q2);
};
#endif
