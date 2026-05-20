#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <atomic>
#include <chrono>
#include <cmath>
#include <thread>

#include "../../state/AirplaneState.h"
class Simulator
{
private:
    // Variables
    AirplaneState* _airplane;
    std::atomic<bool> _simRunning;
    std::thread _simThread;
    static const double _timeScale;
    static const int _tickTime;

    // Méthodes
    void simLoop();
    void computeGroundSpeed();

public:
    // Constructeur
    Simulator(AirplaneState* newAirplane);
    ~Simulator();

    // Encapsulation
    bool getSimRunning();
    int getTickTime();

    // Methodes
    void startSim();
    void stopSim();
};
#endif
