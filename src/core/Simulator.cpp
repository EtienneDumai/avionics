#include "Simulator.h"

Simulator::Simulator(AirplaneState *airplane)
{
    this->_airplane = new AirplaneState(airplane);
}

Simulator::~Simulator()
{
    delete _airplane;
}

void Simulator::simLoop()
{
    while (this->_simRunning)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(32));
    }
}
void Simulator::startSim()
{
    this->_simRunning = true;
    this->_simThread = std::thread(&Simulator::simLoop, this);
}

void Simulator::stopSim()
{
    this->_simRunning = false;
    this->_simThread = std::join()
}
