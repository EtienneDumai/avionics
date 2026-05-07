#ifndef ENGINE_H
#define ENGINE_H
#include <mutex>


class Engine
{
private:
    double  _spoolRate;
    double _engineRPM;
    int _commandRPM;
    int _maxThrust; //in N (Newton)
    bool _state;
    std::mutex mutexAirplaneState;

public:
    double getSpoolRate();
    double getEngineRPM();
    int getCommandRPM();
    int getMaxThrust();
    bool getState();
    void setSpoolRate(double newSpoolRate);
    void setEngineRPM(double newEngineRPM);
    void setCommandRPM(int newCommandRPM);
    void setState(bool newState);
    int computeThrust(double engineRPM, int maxThrust, bool state);
    Engine(double newSpoolRate, double newEngineRPM, int newCommandThrust, int newMaxthrust, bool newState);
    ~Engine();
};

#endif