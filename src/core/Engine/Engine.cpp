#include "Engine.h"

#include <algorithm>

Engine::Engine(double newSpoolRate, double newEngineRPM, int newCommandThrust, int newMaxthrust, bool newState)
{
    this->_spoolRate = newSpoolRate;
    this->_engineRPM = newEngineRPM;
    this->_commandRPM = newCommandThrust;
    this->_maxThrust = newMaxthrust;
    this->_state = newState;
}

Engine::~Engine() {}

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

int Engine::computeThrust()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    int realThrust;
    if (!this->_state)
    {
        realThrust = 0;
    }
    else
    {
        if (this->_engineRPM <= 100)
        {
            realThrust = (this->_engineRPM / 100) * static_cast<double>(this->_maxThrust);
        }
        else
        {
            realThrust = (this->_engineRPM / 100) * static_cast<double>(this->_maxThrust) * 1.25;
        }
    }
    return realThrust;
}

void Engine::updateRPM()
{
    std::lock_guard<std::mutex> lock(this->_mutexEngine);
    if (this->_state)
    {
        if (this->_commandRPM < this->_engineRPM)
        {
            this->_engineRPM = std::max(this->_engineRPM - this->_spoolRate, static_cast<double>(this->_commandRPM));
        }
        else if (this->_commandRPM > this->_engineRPM)
        {
            this->_engineRPM = std::min(this->_engineRPM + this->_spoolRate, static_cast<double>(this->_commandRPM));
        }
        else
        {
            this->_engineRPM = this->_commandRPM;
        }
    }
}
