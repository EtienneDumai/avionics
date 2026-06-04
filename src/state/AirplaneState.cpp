#include "AirplaneState.h"

#include <mutex>

AirplaneState::AirplaneState(int altitude, double xPos, double yPos, double airSpeed, double groundSpeed,
                             double verticalSpeed, double AOA, double heading, int engineCount,
                             EngineConfig newEngineConfig, int newMasse, double newDragCoef, double newSurface)
    : _altitude(altitude),
      _xPos(xPos),
      _yPos(yPos),
      _airSpeed(airSpeed),
      _groundSpeed(groundSpeed),
      _verticalSpeed(verticalSpeed),
      _AOA(AOA),
      _heading(heading),
      _engineCount(engineCount),
      _masse(newMasse),
      _dragCoef(newDragCoef),
      _surface(newSurface)
{
    for (int i = 0; i < engineCount; i++)
    {
        this->_engines.push_back(
            std::make_unique<Engine>(newEngineConfig.spoolRate, 0, 0, newEngineConfig.maxThrust, false));
    }
}
AirplaneState::AirplaneState(AirplaneState* airplane)
{
    this->_airSpeed = airplane->getAirSpeed();
    this->_altitude = airplane->getAltitude();
    this->_AOA = airplane->getAOA();
    this->_groundSpeed = airplane->getGroundSpeed();
    this->_heading = airplane->getHeading();
    this->_verticalSpeed = airplane->getVerticalSpeed();
    this->_xPos = airplane->getXPos();
    this->_yPos = airplane->getYPos();
    this->_masse = airplane->getMasse();
    this->_dragCoef = airplane->getDragCoef();
    this->_surface = airplane->getSurface();
    for (const auto& e : airplane->_engines)
    {
        this->_engines.push_back(std::make_unique<Engine>(e->getSpoolRate(), e->getEngineRPM(), e->getCommandRPM(),
                                                          e->getMaxThrust(), e->getState()));
    }
}
AirplaneState::~AirplaneState() {}

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
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_AOA;
}

double AirplaneState::getHeading()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_heading;
}

double AirplaneState::getEngineRPM(int index)
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    if (this->_engines.empty() || static_cast<int>(this->_engines.size()) <= index || index < 0)
    {
        return -1.0;
    }
    else
    {
        return this->_engines.at(index)->getEngineRPM();
    }
}

int AirplaneState::getEnginesCount()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_engineCount;
}

int AirplaneState::getMasse()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_masse;
}

double AirplaneState::getDragCoef()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_dragCoef;
}

int AirplaneState::getTotalThrust()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    int totalThrust = 0;
    for (int i = 0; i < this->_engineCount; i++)
    {
        totalThrust += this->_engines.at(i)->computeThrust();
    }
    return totalThrust;
}
double AirplaneState::getSurface()
{
    std::lock_guard<std::mutex> lock(this->mutexAirplaneState);
    return this->_surface;
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

void AirplaneState::computeIAS(double groundSpeed)
{
    this->_airSpeed = groundSpeed * pow((1 - 0.0065 * this->_altitude / 288.15), 2.128);
}