#include "AirplaneState.h"

AirplaneState::AirplaneState(int altitude, int xPos, int yPos, int airSpeed, int groundSpeed, double verticalSpeed, double AOA, double heading) : _altitude(altitude), _xPos(xPos), _yPos(yPos), _airSpeed(airSpeed), _groundSpeed(groundSpeed), _verticalSpeed(verticalSpeed), _AOA(AOA), _heading(heading)
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

int AirplaneState::getXPos()
{
    return this->_xPos;
}

int AirplaneState::getYPos()
{
    return this->_yPos;
}

int AirplaneState::getAirSpeed()
{
    return this->_airSpeed;
}

int AirplaneState::getGroundSpeed()
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

void AirplaneState::setXPos(int newXPos)
{
    this->_xPos = newXPos;
}

void AirplaneState::setYPos(int newYPos)
{
    this->_yPos = newYPos;
}

void AirplaneState::setGroundSpeed(int newGroundSpeed)
{
    this->_groundSpeed = newGroundSpeed;
}
