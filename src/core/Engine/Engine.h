#ifndef ENGINE_H
#define ENGINE_H
#include <mutex>
#include <algorithm>
class Engine
{
private:
    // Attributs
    double _spoolRate;
    double _engineRPM;
    int _commandRPM;
    int _maxThrust; // in N (Newton)
    bool _state;
    std::mutex _mutexEngine;

public:
    // Constructeurs
    Engine(double newSpoolRate, double newEngineRPM, int newCommandThrust, int newMaxthrust, bool newState);
    ~Engine();

    // Encapsulation
    double getSpoolRate();
    double getEngineRPM();
    int getCommandRPM();
    int getMaxThrust();
    bool getState();
    void setSpoolRate(double newSpoolRate);
    void setEngineRPM(double newEngineRPM);
    void setCommandRPM(int newCommandRPM);
    void setState(bool newState);

    // Methodes
    int computeThrust();
    int updateRPM();
};

#endif