#include "Engine.h"

Engine::Engine(double newSpoolRate, double newEngineRPM, int newCommandThrust, int newMaxthrust, bool newState)
{
    this->_spoolRate = newSpoolRate;
    this->_engineRPM = newEngineRPM;
    this->_commandRPM = newCommandThrust;
    this->_maxThrust = newMaxthrust;
    this->_state = newState;
}

Engine::~Engine()
{
}

double Engine::getSpoolRate()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    return this->_spoolRate;
}

double Engine::getEngineRPM()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    return this->_engineRPM;
}

int Engine::getCommandRPM()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    return this->_commandRPM;
}

int Engine::getMaxThrust()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    return this->_maxThrust;
}

bool Engine::getState()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    return this->_state;
}

void Engine::setSpoolRate(double newSpoolRate)
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    this->_spoolRate = newSpoolRate;
}

void Engine::setEngineRPM(double newEngineRPM)
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    this->_engineRPM = newEngineRPM;
}

void Engine::setCommandRPM(int newCommandRPM)
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    this->_commandRPM = newCommandRPM;
}

void Engine::setState(bool newState)
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    this->_state = newState;
}

int Engine::computeThrust(double engineRPM, int maxThrust, bool state)
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    int realThrust;
    if (!state)
    {
        realThrust = 0;
    }
    else
    {
        if (engineRPM <= 100)
        {
            realThrust = engineRPM * static_cast<double>(maxThrust);
        }
        else
        {
            realThrust = engineRPM * static_cast<double>(maxThrust) * 1.25;
        }
    }
    return realThrust;
}