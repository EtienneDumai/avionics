#ifndef AIRPLANESTATE_H
#define AIRPLANESTATE_H
#include <math.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include "../core/Engine/Engine.h"
class AirplaneState
{
private:
    int _altitude;        // m
    double _xPos;         // m
    double _yPos;         // m
    double _airSpeed;     // m/s  (IAS — corrigé par la densité atmosphérique)
    double _groundSpeed;  // m/s
    double _verticalSpeed;// m/s
    double _AOA;          // °
    double _heading;      // °
    int _engineCount;
    int _masse;           // kg
    double _dragCoef;     // sans dimension (Cx)
    double _surface;      // m²
    std::vector<std::unique_ptr<Engine>> _engines;
    std::mutex mutexAirplaneState;

public:
    AirplaneState(int altitude, double xPos, double yPos, double airSpeed, double groundSpeed, double verticalSpeed,
                  double AOA, double heading, int engineCount, EngineConfig newEngineConfig, int newMasse,
                  double newDragCoef, double newSurface);
    AirplaneState(AirplaneState* airplane);
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
    int getMasse();
    double getDragCoef();
    double getSurface();
    int getTotalThrust();
    void setAltitude(int newAltitude);
    void setXPos(double newXPos);
    void setYPos(double newYPos);
    void setGroundSpeed(double newGroundSpeed);
    void computeIAS(double groundSpeed);
};
#endif
