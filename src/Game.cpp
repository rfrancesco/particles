#include <iostream>
#include "Game.hpp"
#include "Graphics.hpp"
#include "Input.hpp"
#include "GameLoopTimer.hpp"

#include <algorithm>
#include "Physics.hpp"

Game::Game()
{
    gameLoop();
}

Game::~Game()
{
}

void Game::gameLoop()
{
    Graphics graphics;
    Input input;
    bool running = true;
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
    while (running)
    {
        frametimer.start();
        input.pollInputEvents();
        running = !(input.wasQuitRequested());
        
        dt = frametimer.get_physics_dt();
        Vector2 delta = input.getWASD();

        objects[0].pos.x += 5*delta.x;
        objects[0].pos.y += 5*delta.y;

        if (!input.isWASDHeld())
            objects[0].locked=false;
        else
        {
            objects[0].locked=true;
            objects[0].velocity.x = 0.3f*delta.x/dt;
            objects[0].velocity.y = 0.3f*delta.y/dt;
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
        color.a=255;

        if (object.locked)
            color.b=180;

        graphics.circle(object.pos.x, object.pos.y, object.r, color);
    }
    
    graphics.render();
}