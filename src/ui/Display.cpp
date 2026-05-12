#include "Display.h"
const int Display::_tickTime = 100;
Display::Display(AirplaneState *newAirplaneState)
{
    this->_airplane = newAirplaneState;
    this->_displayRunning = false;
}

Display::~Display()
{
}
int Display::getTickTime()
{
    return this->_tickTime;
}
void Display::startDisplay()
{
    this->_displayRunning = true;
    this->_displayThread = std::thread(&Display::displayLoop, this);
}
void Display::displayLoop()
{
    while (this->_displayRunning)
    {
        std::cout << "Vitesse de l'avion : " << this->_airplane->getGroundSpeed() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->getTickTime()));
    }
}
void Display::stopDisplay()
{
    this->_displayRunning = false;
    this->_displayThread.join();
}