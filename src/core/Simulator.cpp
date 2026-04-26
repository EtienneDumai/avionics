#include "Simulator.h"
const double Simulator::_timeScale = 0.016;
const int Simulator::_tickTime = 16;
Simulator::Simulator(AirplaneState *airplane)
{
    this->_airplane = new AirplaneState(airplane);
    this->_simRunning = false;
}

Simulator::~Simulator()
{
    delete _airplane;
}

void Simulator::simLoop()
{
    while (this->_simRunning)
    {
        this->_airplane->setYPos(this->_airplane->getYPos()+cos(this->_airplane->getHeading()*M_PI/180)*this->_airplane->getGroundSpeed()*(this->_timeScale));
        this->_airplane->setXPos(this->_airplane->getXPos()+sin(this->_airplane->getHeading()*M_PI/180)*this->_airplane->getGroundSpeed()*(this->_timeScale));
        this->_airplane->setAltitude(this->_airplane->getAltitude() + this->_airplane->getVerticalSpeed()*this->_timeScale);
        std::this_thread::sleep_for(std::chrono::milliseconds(this->getTickTime()));
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
    this->_simThread.join();
}

bool Simulator::getSimRunning()
{
    return this->_simRunning;
}

int Simulator::getTickTime()
{
    return this->_tickTime;
}