#include "AirplaneState.h"

AirplaneState::AirplaneState(int altitude, double xPos, double yPos, double airSpeed, double groundSpeed, double verticalSpeed, double AOA, double heading, int engineCount, EngineConfig newEngineConfig) : _altitude(altitude), _xPos(xPos), _yPos(yPos), _airSpeed(airSpeed), _groundSpeed(groundSpeed), _verticalSpeed(verticalSpeed), _AOA(AOA), _heading(heading), _engineCount(engineCount)
{
    for (int i = 0; i < engineCount; i++)
    {
        this->_engines.push_back(std::make_unique<Engine>(newEngineConfig.spoolRate, 0, 0, newEngineConfig.maxThrust, false));
    }
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
    for (const auto &e : airplane->_engines)
    {
        this->_engines.push_back(std::make_unique<Engine>(e->getSpoolRate(), e->getEngineRPM(), e->getCommandRPM(), e->getMaxThrust(), e->getState()));
    }
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
    return this->_engineCount;
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
