#include "Quaternion.h"

Quaternion::Quaternion(double x, double y, double z, double w)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_w = w;
}
Quaternion::Quaternion(double angle, Vec3* axis){
    Vec3 normalized = axis->normalize();
    double sin = std::sin(angle/2);
    this->_x = sin * normalized.getX();
    this->_y = sin * normalized.getY();
    this->_z = sin * normalized.getZ();
    this->_w = std::cos(angle/2);
}
Quaternion::Quaternion(const Quaternion &copy){
    this->_x = copy._x;
    this->_y = copy._y;
    this->_z = copy._z;
    this->_w = copy._w;
}
Quaternion::Quaternion(){
    this->_x = 0.0;
    this->_y = 0.0;
    this->_z = 0.0;
    this->_w = 1.0;
}
double Quaternion::getX() const{
    return this->_x;
}
double Quaternion::getY()const{
    return this->_y;
}
double Quaternion::getZ()const{
    return this->_z;
}
double Quaternion::getW()const{
    return this->_w;
}
void Quaternion::setX(double newX){
    this->_x = newX;
}
void Quaternion::setY(double newY){
    this->_y = newY;
}
void Quaternion::setZ(double newZ){
    this->_z = newZ;
}
void Quaternion::setW(double newW){
    this->_w = newW;
}

Quaternion Quaternion::multiply(const Quaternion& q2){
    double w = this->getW()*q2.getW() - this->getX()*q2.getX() - this->getY()*q2.getY() - this->getZ()*q2.getZ();
    double x = this->getW()*q2.getX() + this->getX()*q2.getW() + this->getY()*q2.getZ() - this->getZ()*q2.getY();
    double y = this->getW()*q2.getY() - this->getX()*q2.getZ() + this->getY()*q2.getW() + this->getZ()*q2.getX();
    double z = this->getW()*q2.getZ() + this->getX()*q2.getY() - this->getY()*q2.getX() + this->getZ()*q2.getW();
    return Quaternion(x,y,z,w);
}
