#ifndef GRAPHICS_SIMULATIONWINDOW_HPP
#define GRAPHICS_SIMULATIONWINDOW_HPP

#include <SDL2/SDL.h>
#include <vector>

struct Color
{
    unsigned int r, g, b, a;
};

class SimulationWindow
{
public:
    SimulationWindow();
    ~SimulationWindow();

    void setBackgroundColor(Color color);
    void clearWindow();
    void rectangle(int x, int y, int w, int h);
    void circle(int x_center, int y_center, int radius, Color color);
    void render();

    int getWidth();
    int getHeight();

    void handleWindowResize();

    // private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *window_surface;

    Color bg_color;
};

#endif
