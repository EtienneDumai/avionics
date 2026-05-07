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
    return this->_spoolRate;
}

double Engine::getEngineRPM()
{
    return this->_engineRPM;
}

int Engine::getCommandRPM()
{
    return this->_commandRPM;
}

int Engine::getMaxThrust()
{
    return this->_maxThrust;
}

bool Engine::getState()
{
    return this->_state;
}

void Engine::setSpoolRate(double newSpoolRate)
{
    this->_spoolRate = newSpoolRate;
}

void Engine::setEngineRPM(double newEngineRPM)
{
    this->_engineRPM = newEngineRPM;
}

void Engine::setCommandRPM(int newCommandRPM)
{   
    this->_commandRPM = newCommandRPM;
}

void Engine::setState(bool newState)
{
    this->_state = newState;
}

int Engine::computeThrust(double engineRPM, int maxThrust, bool state)
{
    int realThrust;
    if (!state)
    {
        realThrust = 0;
    }
    else
    {
        if (engineRPM<=100)
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