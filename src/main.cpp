#include <chrono>

#include "core/Simulator/Simulator.h"
#include "state/AirplaneState.h"
#include "ui/Display/Display.h"
#include "ui/Window/Window.h"

int main()
{
    AirplaneState airplane(150, 0, 0, 150, 175, 0, 0, 0, 1, {0.2, 15000}, 70000, 0.03, 122);
    Simulator simulator(&airplane);
    Window window(&airplane, 300, 300, "Avionics");
    simulator.startSim();
    window.run();
    simulator.stopSim();
    return 0;
}
