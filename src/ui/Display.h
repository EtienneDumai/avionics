#ifndef DISPLAY_H
#define DISPLAY_H
#include "core/Simulator.h"
#include <atomic>
class Display
{
private:
    Simulator *simulator;
    std::atomic<bool> displayRunning;
public:
    Display(/* args */);
    ~Display();
    void startDisplay();
    void stopDisplay();
};
#endif