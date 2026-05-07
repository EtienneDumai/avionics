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
    std::mutex mutexAirplaneState;

public:
    Engine(/* args */);
    ~Engine();
};

Engine::Engine(/* args */)
{
}

Engine::~Engine()
{
}


#endif