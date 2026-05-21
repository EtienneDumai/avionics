#ifndef WINDOW_H
#define WINDOW_H

#include "../../state/AirplaneState.h"
#include "SDL2/SDL.h"
class Window
{
private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    AirplaneState* _airplane;
    static const int _tickTime;

public:
    int _height;
    int _width;
    void run();
    Window(AirplaneState* newAirplane);

    ~Window();
};
#endif  // !WINDOW_H
