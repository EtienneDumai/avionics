#ifndef AIRPLANESTATE_H
#define AIRPLANESTATE_H
#include <math.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include "../core/Engine/Engine.h"
#include "../math/Quaternion/Quaternion.h"

class AirplaneState
{
private:
    double _altitude;        // m
    double _xPos;         // m
    double _yPos;         // m
    double _airSpeed;     // m/s  (IAS — corrigé par la densité atmosphérique)
    Vec3 _velocity;
    double _AOA;          // °
    Quaternion _orientation;
    int _engineCount;
    int _masse;           // kg
    double _dragCoef;     // sans dimension (Cx)
    double _liftCoef;     // sans dimensions (Cl)
    double _surface;      // m²
    std::vector<std::unique_ptr<Engine>> _engines;
    std::mutex mutexAirplaneState;

public:
    AirplaneState(double altitude, double xPos, double yPos, double airSpeed, Vec3 velocity, double AOA, Quaternion orientation, int engineCount,
                    EngineConfig newEngineConfig, int newMasse, double newDragCoef,  double newLiftCoef, double newSurface);
    AirplaneState(AirplaneState* airplane);
    ~AirplaneState();
    double getAltitude();
    double getXPos();
    double getYPos();
    double getAirSpeed();
    Vec3 getVelocity();
    double getAOA();
    Vec3 getForward();
    Quaternion getOrientation();
    double getHeading();
    double getEngineRPM(int index);
    int getEnginesCount();
    int getMasse();
    double getDragCoef();
    double getSurface();
    double getLiftCoef();
    int getTotalThrust();
    void setAltitude(double newAltitude);
    void setXPos(double newXPos);
    void setYPos(double newYPos);
    void setVelocity(Vec3 newVelocity);
    void setAOA(double newAOA);
    void computeIAS(double groundSpeed);
    void computeAOA(Vec3 forwardVec, Vec3 velocityVec);
};
#endif
