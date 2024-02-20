#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "Graphics/SimulationWindow.hpp"
#include "SDLManager.hpp"
#include <numeric>
#include <algorithm>

static void if_true_crash(bool test, std::string msg)
{
    if (test)
    {
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

SimulationWindow::SimulationWindow()
{
    SDLManager::initSDLIfNotYetInitialized();
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              300, 300, window_flags);
    if_true_crash(!window, "Could not create SDL Window!");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if_true_crash(!renderer, "Could not create SDL Renderer!");
    window_surface = SDL_GetWindowSurface(window);
    if_true_crash(!window_surface, "Could not get SDL Surface from main window");
    set_bg_color({255, 201, 211, 255});
    clear_window();
    render();
}

SimulationWindow::~SimulationWindow()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void SimulationWindow::set_bg_color(Color color)
{
    bg_color = color;
}

void SimulationWindow::clear_window()
{
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 255);
    SDL_RenderClear(renderer);
}

void SimulationWindow::render()
{
    SDL_RenderPresent(renderer);
}

// static std::vector<double> smooth(const std::vector<double> &in, unsigned int n)
// {
//     std::vector<double> out(in.size());
//     for (size_t i = 0; i < in.size(); ++i)
//     {
//         size_t r = (i + n) < in.size() ? i + n : in.size() - 1;
//         size_t l = ((i - n) < r) ? (i - n) : 0; // size_t is unsigned!
//         size_t diff = r - l;

//         double avg = 0.;
//         for (; l < r; ++l)
//             avg += in[r];

//         out[i] = avg / diff;
//     }
//     std::cout << out[out.size() - 1] << std::endl;
//     return out;
// }

int SimulationWindow::get_width()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return w;
}

int SimulationWindow::get_height()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return h;
}

void SimulationWindow::handleWindowResize()
{
    SDL_FreeSurface(window_surface);
    window_surface = SDL_GetWindowSurface(window);
}

void SimulationWindow::circle(int x_center, int y_center, int radius, Color color)
{
    if (radius <= 0)
        return;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    std::vector<SDL_Point> points;
    for (int x = 0; x < radius * 2; x++)
    {
        for (int y = 0; y < radius * 2; y++)
        {
            int dx = radius - x;
            int dy = radius - y;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                points.push_back({x_center + dx, y_center + dy});
            }
        }
    }
    SDL_RenderDrawPoints(renderer, points.data(), points.size());
}
