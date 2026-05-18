#include <iostream>
#include <chrono>
#include "state/AirplaneState.h"
#include "core/Simulator/Simulator.h"
#include "ui/Display.h"

int main()
{
    AirplaneState airplane(150, 0,0,150,175,0,0,0,1,{0.2, 15000});
    Simulator simulator(&airplane);
    Display display(&airplane);
    simulator.startSim();
    display.startDisplay();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    simulator.stopSim();
    display.stopDisplay();
    return 0;
}