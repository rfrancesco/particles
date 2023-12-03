#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "Graphics.hpp"


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
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                    640,480,window_flags);
    if_true_crash(!window, "Could not create SDL Window!");
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if_true_crash(!renderer, "Could not create SDL Renderer!");
    window_surface = SDL_GetWindowSurface(window);
    if_true_crash(!window_surface,"Could not get SDL Surface from main window");
    set_bg_color({255,201,211});
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
    //SDL_FillRect( window_surface, NULL, SDL_MapRGB( window_surface->format, bg_color.r, bg_color.g, bg_color.b ));
}

void Graphics::render()
{
    SDL_RenderPresent(renderer);
    // SDL_UpdateWindowSurface(window);
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

/* From StackOverflow 
 * https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl */
static int roundUpToMultipleOfEight( int v )
{
    return (v + (8 - 1)) & -8;
}
void Graphics::emptyCircle(int x_center, int y_center, int radius)
{
    // 35 / 49 is a slightly biased approximation of 1/sqrt(2)
    const int arrSize = roundUpToMultipleOfEight( radius * 8 * 35 / 49 );
    std::vector<SDL_Point> points;
    SDL_Point center;
    center.x = x_center;
    center.y = y_center;

    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    SDL_SetRenderDrawColor(renderer,255,10,10,255);

    while( x >= y )
    {
        // Each of the following renders an octant of the circle
        points.push_back({ center.x + x, center.y - y });
        points.push_back({ center.x + x, center.y + y });
        points.push_back({ center.x - x, center.y - y });
        points.push_back({ center.x - x, center.y + y });
        points.push_back({ center.x + y, center.y - x });
        points.push_back({ center.x + y, center.y + x });
        points.push_back({ center.x - y, center.y - x });
        points.push_back({ center.x - y, center.y + x });

        if( error <= 0 )
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if( error > 0 )
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }

    std::cout << "points: " <<  points.size() << std::endl;

    SDL_RenderDrawPoints( renderer, points.data(), points.size() );
}

void Graphics::circle(int x_center, int y_center, int radius, Color color)
{
    if (radius<=0)
        return;

    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,255);

    std::vector<SDL_Point> points;

    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                points.push_back({x_center+dx,y_center+dy});
            }
        }
    }
    SDL_RenderDrawPoints( renderer, points.data(), points.size() );
}