#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "../state/AirplaneState.h"
#include <thread>
#include <atomic>
#include <chrono>
class Simulator
{
private:
    // Variables
    AirplaneState *_airplane;
    std::atomic<bool> _simRunning;
    std::thread _simThread;
    // Méthodes
    void simLoop();

public:
    // Constructeur
    Simulator(AirplaneState *newAirplane);
    ~Simulator();
    // Encapsulation
    AirplaneState *getAirplaneState();
    std::atomic<bool> getSimRunning();
    // Methodes
    void startSim();
    void stopSim();
};
#endif
