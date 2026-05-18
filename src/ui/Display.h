#ifndef DISPLAY_H
#define DISPLAY_H
#include "../state/AirplaneState.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
class Display
{
private:
    // Variables
    AirplaneState *_airplane;
    std::atomic<bool> _displayRunning;
    std::thread _displayThread;
    static const int _tickTime;

    // Méthodes
    void displayLoop();

public:
    // Constructeur
    Display(AirplaneState *newAirplane);
    ~Display();
    int getTickTime();
    // Méthodes
    void startDisplay();
    void stopDisplay();
};
#endif