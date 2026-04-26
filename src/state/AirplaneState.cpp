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
    return this->_altitude;
}

double AirplaneState::getXPos()
{
    return this->_xPos;
}

double AirplaneState::getYPos()
{
    return this->_yPos;
}

double AirplaneState::getAirSpeed()
{
    return this->_airSpeed;
}

double AirplaneState::getGroundSpeed()
{
    return this->_groundSpeed;
}

double AirplaneState::getVerticalSpeed()
{
    return this->_verticalSpeed;
}

double AirplaneState::getAOA()
{
    return this->_AOA;
}

double AirplaneState::getHeading()
{
    return this->_heading;
}

void AirplaneState::setAltitude(int newAltitude)
{
    this->_altitude = newAltitude;
}

void AirplaneState::setXPos(double newXPos)
{
    this->_xPos = newXPos;
}

void AirplaneState::setYPos(double newYPos)
{
    this->_yPos = newYPos;
}

void AirplaneState::setGroundSpeed(double newGroundSpeed)
{
    this->_groundSpeed = newGroundSpeed;
}
