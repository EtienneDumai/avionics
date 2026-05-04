#include <iostream>
#include "state/AirplaneState.h"
#include "core/Simulator.h"
int main()
{
    AirplaneState *airplane = new AirplaneState(2, 0, 0, 250, 300, 0, 0, 360);
    Simulator *simu = new Simulator(airplane);
    simu->startSim();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    simu->stopSim();
    delete airplane;
    delete simu;
    return 0;
}