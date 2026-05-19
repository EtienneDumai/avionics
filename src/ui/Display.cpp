#include "Display.h"
const int Display::_tickTime = 100;
Display::Display(AirplaneState *newAirplaneState)
{
    this->_airplane = newAirplaneState;
    this->_displayRunning = false;
}

Display::~Display()
{
    if (this->_displayThread.joinable())
    {
        this->stopDisplay();
    }
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
        // clear terminal
        std::cout << "\033[2J\033[H";
        std::cout << "Heading : " << this->_airplane->getHeading() << std::endl;
        std::cout << "xPos : " << this->_airplane->getXPos() << std::endl;
        std::cout << "yPos : " << this->_airplane->getYPos() << std::endl;
        std::cout << "ASPD : " << this->_airplane->getAirSpeed() << std::endl;
        std::cout << "GSPD : " << this->_airplane->getGroundSpeed() << std::endl;
        std::cout << "AOA : " << this->_airplane->getAOA() << std::endl;
        std::cout << "ALT : " << this->_airplane->getAltitude() << std::endl;
        for (int i = 0; i < this->_airplane->getEnginesCount(); i++)
        {
            std::cout << "ENG" << i + 1 << " : " << this->_airplane->getEngineRPM(i) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->getTickTime()));
    }
}
void Display::stopDisplay()
{
    this->_displayRunning = false;
    this->_displayThread.join();
}