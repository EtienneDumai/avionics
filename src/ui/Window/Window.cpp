#include "Window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include <sstream>
#include <string>

Window::Window(AirplaneState* newAirplane, int newWidth, int newHeight, const char* title)
{
    this->_airplane = newAirplane;
    this->_width = newWidth;
    this->_height = newHeight;
    SDL_Init(SDL_INIT_VIDEO);
    this->_window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->_width, this->_height, 0);
    this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Init();
    this->_font = TTF_OpenFont("assets/font/ShareTechMono-Regular.ttf", 18);
}
Window::~Window()
{
    if (this->_renderer)
    {
        SDL_DestroyRenderer(this->_renderer);
    }
    if (this->_window)
    {
        SDL_DestroyWindow(this->_window);
    }
    if (this->_font)
    {
        TTF_CloseFont(this->_font);
    }
    TTF_Quit();
    SDL_Quit();
}

void Window::run()
{
    SDL_Event event;
    bool isRunning = true;
    SDL_Color green = {0,255,0,SDL_ALPHA_OPAQUE};
    SDL_Color red = {255,0,0,SDL_ALPHA_OPAQUE};
    while (isRunning)
    {
        std::vector<DataColor> tabVal;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }
        tabVal.push_back({"GSPD : " + std::to_string(this->_airplane->getGroundSpeed()) + "m/s",green});
        tabVal.push_back({"HEAD : " + std::to_string(this->_airplane->getHeading()) + " °", green});
        tabVal.push_back({"ALT : " + std::to_string(this->_airplane->getAltitude()) + "m", green});
        for (int i = 0; i <= static_cast<int>(this->_airplane->getEnginesCount()); i++)
        {
            if (this->_airplane->getEngineRPM(i)<=100)
            {
                tabVal.push_back({"ENG"+ std::to_string(i+1)+" : " + std::to_string(this->_airplane->getEngineRPM(i)) + "%",green});
            }
            else
            {
                tabVal.push_back({"ENG"+ std::to_string(i+1)+" : " + std::to_string(this->_airplane->getEngineRPM(i)) + "%",red});
            }
            
        }
        tabVal.push_back({"XPOS : " + std::to_string(this->_airplane->getXPos()),green});
        tabVal.push_back({"YPOS : " + std::to_string(this->_airplane->getYPos()),green});
        tabVal.push_back({"AOA : " + std::to_string(this->_airplane->getAOA())+" °", green});
        SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(this->_renderer);
        SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        for (int i = 0; i < static_cast<int>(tabVal.size()); i++)
        {
            this->displayInfos(tabVal.at(i), i+1);
        }
        SDL_RenderPresent(this->_renderer);
        tabVal.clear();
    }
}

void Window::displayInfos(std::string info, int lineIndex, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Solid(this->_font, info.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(this->_renderer, surface);
    SDL_FreeSurface(surface);
    int w, h;
    const int paddingTop = 10;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = {100, paddingTop + lineIndex * TTF_FontHeight(this->_font), w, h};
    SDL_RenderCopy(this->_renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}
