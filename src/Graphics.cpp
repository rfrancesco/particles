#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "Graphics.hpp"
#include "SDLManager.hpp"


static void if_true_crash(bool test, std::string msg)
{
    if (test)
    {
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

Graphics::Graphics()
{
    SDLManager::initSDLIfNotYetInitialized();
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                    300,300,window_flags);
    if_true_crash(!window, "Could not create SDL Window!");
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if_true_crash(!renderer, "Could not create SDL Renderer!");
    window_surface = SDL_GetWindowSurface(window);
    if_true_crash(!window_surface,"Could not get SDL Surface from main window");
    set_bg_color({255,201,211,255});
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2"  );
    SDL_SetHint( SDL_HINT_RENDER_VSYNC, "2"  );
    clear_window();
    render();
}

Graphics::~Graphics()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Graphics::set_bg_color(Color color)
{
    bg_color = color;
}

void Graphics::clear_window()
{
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 255);
    SDL_RenderClear(renderer);
}

void Graphics::render()
{
    SDL_RenderPresent(renderer);
}

int Graphics::get_width()
{
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    return w;
}

int Graphics::get_height()
{
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    return h;
}

void Graphics::handleWindowResize()
{
    SDL_FreeSurface(window_surface);
    window_surface = SDL_GetWindowSurface(window);
}

void Graphics::circle(int x_center, int y_center, int radius, Color color)
{
    if (radius<=0)
        return;

    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

    std::vector<SDL_Point> points;
    for (int x = 0; x < radius * 2; x++)
    {
        for (int y = 0; y < radius * 2; y++)
        {
            int dx = radius - x; 
            int dy = radius - y; 
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                points.push_back({x_center+dx,y_center+dy});
            }
        }
    }
    SDL_RenderDrawPoints( renderer, points.data(), points.size() );
}