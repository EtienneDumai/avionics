#include "Vec3.h"
Vec3::Vec3(double newX, double newY, double newZ)
{
    this->_x = newX;
    this->_y = newY;
    this->_z = newZ;
}
Vec3::Vec3(const Vec3& newVec3)
{
    this->_x = newVec3.getX();
    this->_y = newVec3.getY();
    this->_z = newVec3.getZ();
}
Vec3::Vec3()
{
    // Vecteur nul
    this->_x = 0.0;
    this->_y = 0.0;
    this->_z = 0.0;
}
Vec3::~Vec3() {}
double Vec3::getX() const { return this->_x; }
double Vec3::getY() const { return this->_y; }
double Vec3::getZ() const { return this->_z; }
void Vec3::setX(double newX) { this->_x = newX; }
void Vec3::setY(double newY) { this->_y = newY; }
void Vec3::setZ(double newZ) { this->_z = newZ; }

Vec3 Vec3::normalize()
{
    double vectLength = std::sqrt(std::pow(this->getX(), 2) + std::pow(this->getY(), 2) + std::pow(this->getZ(), 2));
    if (vectLength < 1e-9)
    {
        return Vec3(0.0, 0.0, 0.0);
    }
    return Vec3(this->getX() / vectLength, this->getY() / vectLength, this->getZ() / vectLength);
}
