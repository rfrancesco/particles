#include <iostream>
#include "GameLoopTimer.hpp"

void GameLoopTimer::start()
{
    t_start = SDL_GetPerformanceCounter();
}

void GameLoopTimer::end()
{
    t_end = SDL_GetPerformanceCounter();
    current_fps = 1/elapsed_time();
}

double GameLoopTimer::elapsed_time()
{
    return (t_end - t_start) / static_cast<double>(SDL_GetPerformanceFrequency());
}

void GameLoopTimer::capFPS(Uint64 max_fps)
{
    double t_elapsed = elapsed_time();
    current_fps = 1/t_elapsed;
    if (current_fps > max_fps)
        SDL_Delay((1.0/max_fps - t_elapsed)*1000);

    end();
    current_fps = 1.0/elapsed_time();
}

void GameLoopTimer::logFPS() const
{
    std::cout << "FPS = " << current_fps << std::endl;
}

double GameLoopTimer::get_physics_dt()
{
    double dt;
    t_physics_now = SDL_GetPerformanceCounter();
    dt = (t_physics_now - t_physics_last) / static_cast<double>(SDL_GetPerformanceFrequency());
    t_physics_last = t_physics_now;
    return dt;
}