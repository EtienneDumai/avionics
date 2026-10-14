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
    static const double _g;

    // Méthodes
    void simLoop();
    double computeAirDensity(double altitude);
    Vec3 computeThrust(const Vec3& forward);
    Vec3 computeDrag(const Vec3& v, double rho);
    Vec3 computeLift(const Vec3& v, const Vec3& right, double rho);
    Vec3 computeWeight(double m) ;

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
