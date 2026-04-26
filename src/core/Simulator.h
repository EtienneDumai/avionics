#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "../state/AirplaneState.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <cmath>
class Simulator
{

private:

    // Variables
    AirplaneState *_airplane;
    std::atomic<bool> _simRunning;
    std::thread _simThread;
    static const double _timeScale;
    static const int _tickTime;

    // Méthodes
    void simLoop();

public:

    // Constructeur
    Simulator(AirplaneState *newAirplane);
    ~Simulator();

    // Encapsulation
    AirplaneState *getAirplaneState();
    bool getSimRunning();
    int getTickTime();

    // Methodes
    void startSim();
    void stopSim();
};
#endif
