#include <iostream>
using namespace std;

class AirplaneState
{
private:
    int _altitude;
    int _xPos;
    int _yPos;
    int _airSpeed;
    int _groundSpeed;
    double _verticalSpeed;
    double _AOA;
    double _heading;

public:
    AirplaneState(int altitude, int xPos, int yPos, int airSpeed, int groundSpeed, double verticalSpeed, double AOA, double heading);
    AirplaneState(AirplaneState *airplane);
    ~AirplaneState();
    int getAltitude();
    int getXPos();
    int getYPos();
    int getAirSpeed();
    int getGroundSpeed();
    double getVerticalSpeed();
    double getAOA();
    double getHeading();
    void setAltitude(int newAltitude);
    void setXPos(int newXPos);
    void setYPos(int newYPos);
    void setGroundSpeed(int newGroundSpeed);
};
