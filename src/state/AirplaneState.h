#ifndef AIRPLANESTATE_H
#define AIRPLANESTATE_H
#include <iostream>
using namespace std;

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

public:
    AirplaneState(int altitude, double xPos, double yPos, double airSpeed, double groundSpeed, double verticalSpeed, double AOA, double heading);
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
    void setAltitude(int newAltitude);
    void setXPos(double newXPos);
    void setYPos(double newYPos);
    void setGroundSpeed(double newGroundSpeed);
};
#endif
