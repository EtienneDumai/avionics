#ifndef WINDOW_H
#define WINDOW_H

#include "../../state/AirplaneState.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
class Window
{
private:
    TTF_Font* _font;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    AirplaneState* _airplane;
    static const int _tickTime;
    void displayInfos(std::string info, int lineIndex);

public:
    int _height;
    int _width;
    void run();
    Window(AirplaneState* newAirplane, int newWidth, int newHeight, const char* title);

    ~Window();
};
#endif  // !WINDOW_H
