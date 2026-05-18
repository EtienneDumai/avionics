#ifndef AIRPLANESTATE_H
#define AIRPLANESTATE_H
#include <iostream>
#include <mutex>
#include "../core/Engine/Engine.h"
#include <vector>
#include <memory>
class AirplaneState
{
private:
    int _altitude;
    double _xPos;
    double _yPos;
    double _airSpeed;
    double _groundSpeed;
    double _verticalSpeed;
    double _AOA;
    double _heading;
    int _engineCount;
    std::vector<std::unique_ptr<Engine>> _engines;
    std::mutex mutexAirplaneState;

public:
    AirplaneState(int altitude, double xPos, double yPos, double airSpeed, double groundSpeed, double verticalSpeed, double AOA, double heading, int engineCount, EngineConfig newEngineConfig);
    AirplaneState(AirplaneState *airplane);
    ~AirplaneState();
    int getAltitude();
    double getXPos();
    double getYPos();
    double getAirSpeed();
    double getGroundSpeed();
    double getVerticalSpeed();
    double getAOA();
    double getHeading();
    double getEngineRPM(int index);
    int getEnginesCount();
    void setAltitude(int newAltitude);
    void setXPos(double newXPos);
    void setYPos(double newYPos);
    void setGroundSpeed(double newGroundSpeed);
};
#endif
