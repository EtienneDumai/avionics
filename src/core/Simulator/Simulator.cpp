#include "Simulator.h"

const double Simulator::_timeScale = 0.016;
const int Simulator::_tickTime = 16;
Simulator::Simulator(AirplaneState* airplane)
{
    this->_airplane = airplane;
    this->_simRunning = false;
}

Simulator::~Simulator()
{
    if (this->_simThread.joinable())
    {
        this->_simThread.join();
    }
}

void Simulator::simLoop()
{
    while (this->_simRunning)
    {
        this->computeGroundSpeed();
        this->_airplane->computeIAS(this->_airplane->getGroundSpeed());
        this->_airplane->setYPos(this->_airplane->getYPos() + cos(this->_airplane->getHeading() * M_PI / 180) *
                                                                  this->_airplane->getGroundSpeed() *
                                                                  (this->_timeScale));
        this->_airplane->setXPos(this->_airplane->getXPos() + sin(this->_airplane->getHeading() * M_PI / 180) *
                                                                  this->_airplane->getGroundSpeed() *
                                                                  (this->_timeScale));
        this->_airplane->setAltitude(this->_airplane->getAltitude() +
                                     this->_airplane->getVerticalSpeed() * this->_timeScale);
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
void Simulator::computeGroundSpeed()
{
    const double RHO = 1.225 * pow((1 - 0.0065 * this->_airplane->getAltitude() / 288.15), 4.256);
    const double F_DRAG = 0.5 * RHO * pow(this->_airplane->getGroundSpeed(), 2) * this->_airplane->getDragCoef() *
                          this->_airplane->getSurface();
    const double F_NETTE = this->_airplane->getTotalThrust() - F_DRAG;
    const double A = F_NETTE / this->_airplane->getMasse();
    this->_airplane->setGroundSpeed(this->_airplane->getGroundSpeed() + A * this->_timeScale);
}
bool Simulator::getSimRunning() { return this->_simRunning; }

int Simulator::getTickTime() { return this->_tickTime; }
