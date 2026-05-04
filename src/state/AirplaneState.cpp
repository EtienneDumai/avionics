#include "AirplaneState.h"

AirplaneState::AirplaneState(int altitude, double xPos, double yPos, double airSpeed, double groundSpeed, double verticalSpeed, double AOA, double heading) : _altitude(altitude), _xPos(xPos), _yPos(yPos), _airSpeed(airSpeed), _groundSpeed(groundSpeed), _verticalSpeed(verticalSpeed), _AOA(AOA), _heading(heading)
{
}
AirplaneState::AirplaneState(AirplaneState *airplane)
{
    this->_airSpeed = airplane->getAirSpeed();
    this->_altitude = airplane->getAltitude();
    this->_AOA = airplane->getAOA();
    this->_groundSpeed = airplane->getGroundSpeed();
    this->_heading = airplane->getHeading();
    this->_verticalSpeed = airplane->getVerticalSpeed();
    this->_xPos = airplane->getXPos();
    this->_yPos = airplane->getYPos();
}
AirplaneState::~AirplaneState()
{
}

int AirplaneState::getAltitude()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_altitude;
}

double AirplaneState::getXPos()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_xPos;
}

double AirplaneState::getYPos()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_yPos;
}

double AirplaneState::getAirSpeed()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_airSpeed;
}

double AirplaneState::getGroundSpeed()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_groundSpeed;
}

double AirplaneState::getVerticalSpeed()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_verticalSpeed;
}

double AirplaneState::getAOA()
{
    return this->_AOA;
}

double AirplaneState::getHeading()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_heading;
}

void AirplaneState::setAltitude(int newAltitude)
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    this->_altitude = newAltitude;
}

void AirplaneState::setXPos(double newXPos)
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    this->_xPos = newXPos;
}

void AirplaneState::setYPos(double newYPos)
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    this->_yPos = newYPos;
}

void AirplaneState::setGroundSpeed(double newGroundSpeed)
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    this->_groundSpeed = newGroundSpeed;
}
