#include <iostream>
#include "Game.hpp"
#include "Graphics.hpp"
#include "Input.hpp"
#include "GameLoopTimer.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include "Physics.hpp"

Game::Game()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "Game: Cannot initialize SDL2!" << std::endl;
        exit(EXIT_FAILURE);
    }

    gameLoop();
}

Game::~Game()
{
    SDL_Quit();
}

void Game::gameLoop()
{
    Graphics graphics;
    Input input;
    SDL_Event event;
    GameLoopTimer frametimer;
    PhysicsEngine physics;
    double dt;

    for (int i=0; i<50; i++)
    {
        objects.push_back(GameObject());
        objects[i].pos = {300+rand()%100, 100+rand()%100};
        objects[i].velocity = {rand()%100-50, 0.0};
        objects[i].force = {0.0,0.0};
        objects[i].r = 12;
    }

    (void) frametimer.get_physics_dt();
    while (true)
    {
        frametimer.start();
        input.beginNewFrame();
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
                input.keyDownEvent(event);
            else if (event.type == SDL_KEYUP)
                input.keyUpEvent(event);
            else if (event.type == SDL_WINDOWEVENT)
                graphics.handleWindowResize();

            if (event.type == SDL_QUIT)
                return;
        }
        if (input.wasKeyPressed(SDL_SCANCODE_ESCAPE))
            return;

        dt = frametimer.get_physics_dt();
        int delta[2] = {0,0};
        if (input.isKeyHeld(SDL_SCANCODE_W))
            delta[1] -= 5;
        else if (input.isKeyHeld(SDL_SCANCODE_S))
            delta[1] += 5;
        if (input.isKeyHeld(SDL_SCANCODE_D))
            delta[0] += 5;
        else if (input.isKeyHeld(SDL_SCANCODE_A))
            delta[0] -= 5;

        objects[0].pos.x += delta[0];
        objects[0].pos.y += delta[1];

        if (!input.isKeyHeld(SDL_SCANCODE_W) 
            && !input.isKeyHeld(SDL_SCANCODE_A) 
            && !input.isKeyHeld(SDL_SCANCODE_S) 
            && !input.isKeyHeld(SDL_SCANCODE_D) )
            objects[0].locked=false;
        else
        {
            objects[0].locked=true;
            objects[0].velocity.x = 0.3f*delta[0]/dt;
            objects[0].velocity.y = 0.3f*delta[1]/dt;
        }

        physics.computeForces(objects);
        physics.evolveObjects(objects,dt, graphics.get_width(), graphics.get_height());
        frametimer.end();
        //std::cout << "Time for logic:" << frametimer.elapsed_time() << std::endl;

        draw(graphics);

        frametimer.end();
        //std::cout << "Time for logic+render:" << frametimer.elapsed_time() << std::endl;
        frametimer.capFPS(FPS);
        frametimer.logFPS();
    }
}

void Game::draw(Graphics& graphics)
{   
    graphics.clear_window();

    for (auto& object : objects)
    {
        Color color;
        color.r=std::min(255, static_cast<int>(object.velocity.norm())/2);
        color.g=0;
        color.b=0;

        graphics.circle(object.pos.x, object.pos.y, object.r, color);

        /*SDL_FillRect( graphics.window_surface, &rectangle, SDL_MapRGB( graphics.window_surface->format, color.r, color.g, color.b ));*/
    }
    
    graphics.render();
}