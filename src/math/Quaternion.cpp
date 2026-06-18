#include "Quaternion.h"

Quaternion::Quaternion(double x, double y, double z, double w)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_w = w;
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
