#include <chrono>

#include "core/Simulator/Simulator.h"
#include "math/Quaternion/Quaternion.h"
#include "math/Vec3/Vec3.h"
#include "state/AirplaneState.h"
#include "ui/Display/Display.h"
#include "ui/Window/Window.h"

int main()
{
    Vec3 axis(0,0,1);
    Vec3 axis2(0,1,0);
    Quaternion q2(-M_PI/2,&axis);
    q2.rotate(axis2);
    Vec3 result(q2.rotate(axis2));

    std::cout << "x : " << result.getX() << " y : " << result.getY() << " z : " << result.getZ() << std::endl;
    AirplaneState airplane(150.00, 0, 0, 150, 175, 0, 0, 0, 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
    Simulator simulator(&airplane);
    Window window(&airplane, 300, 300, "Avionics");
    simulator.startSim();
    window.run();
    simulator.stopSim();
    return 0;
}
