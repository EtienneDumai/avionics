#include "Window.h"

#include <SDL2/SDL_events.h>

Window::Window(AirplaneState* newAirplane) { this->_airplane = newAirplane; }
Window::~Window() {}

void Window::run()
{
    SDL_Event event;
    bool isRunning = true;
    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }
    }
}
