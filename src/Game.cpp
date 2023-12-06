#include <iostream>
#include "Game.hpp"
#include "Graphics.hpp"
#include "Input.hpp"
#include "GameLoopTimer.hpp"

#include <algorithm>
#include <iomanip>
#include <deque>
#include <numeric>
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
    unsigned long int count = 0;
    std::deque<double> p, a, pa, ke;
    std::vector<float> vpa, vp, va;

    for (int i = 0; i < 200; i++)
    {
        objects.push_back(GameObject());
        objects[i].pos = {rand() % graphics.get_width(), rand() % graphics.get_height()};
        objects[i].velocity = {rand() % 300 - 150, rand() % 300 - 150};
        objects[i].force = {0.0, 0.0};
        objects[i].r = 4;
    }

    (void)frametimer.get_physics_dt();
    while (running)
    {
        count++;
        frametimer.start();
        input.pollInputEvents();
        running = !(input.wasQuitRequested());
        if (input.wasWindowResized())
            physics.setWorldBox(graphics.get_width(), graphics.get_height());

        Vector2 delta = input.getWASD();

        objects[0].pos.x += 5 * delta.x;
        objects[0].pos.y += 5 * delta.y;

        if (!input.isWASDHeld())
            objects[0].locked = false;
        else
        {
            objects[0].locked = true;
            objects[0].velocity.x = 0.3f * delta.x / dt;
            objects[0].velocity.y = 0.3f * delta.y / dt;
        }

        dt = frametimer.get_physics_dt();
        // physics.computeForces(objects);
        physics.evolveObjects(objects, dt);
        frametimer.end();
        // std::cout << "Time for logic:" << frametimer.elapsed_time() << std::endl;

        if (count > 200)
        {
            double pressure = physics.getPressure();
            double area = physics.getVanDerWaalsArea(objects);
            p.push_back(pressure);
            a.push_back(area);
            pa.push_back(pressure * area);
            ke.push_back(physics.getTotalKineticEnergy(objects));

            pressure = std::accumulate(p.begin(), p.end(), 0.0) / p.size();
            double pressurearea = std::accumulate(pa.begin(), pa.end(), 0.0) / pa.size();
            double kinetic = std::accumulate(ke.begin(), ke.end(), 0.0) / ke.size();
            area = std::accumulate(a.begin(), a.end(), 0.0) / a.size();
            if (p.size() > 250)
            {
                p.pop_front();
                a.pop_front();
                pa.pop_front();
                ke.pop_front();
            }
            // std::cout
            // 	  <<  "Thermodynamics:\tP = " << pressure
            // 	  << "\tA = " << area
            // 	  << "\tPA = " << pressurearea/1.e7
            // 	  << "\tE = " << kinetic << "\t size=" << p.size() << std::endl;
            vpa.push_back(pressurearea / 1.e7);
            vp.push_back(pressure / 1e2);
            va.push_back(area / 1e5);
            std::vector<std::vector<float>> plots;
            plots.push_back(vpa);
            plots.push_back(vp);
            plots.push_back(va);
            graphics.renderImGuiWindow_plots(plots);
        }
        else
            graphics.renderImGuiWindow_thermalization(count, 200);
        draw(graphics);

        frametimer.end();
        // std::cout << "Time for logic+render:" << frametimer.elapsed_time() << std::endl;
        frametimer.capFPS(FPS);
        frametimer.logFPS();
    }
}

void Game::draw(Graphics &graphics)
{
    graphics.clear_window();

    // std::cout << "Objects" << std::endl;
    for (auto &object : objects)
    {
        Color color;
        color.r = std::min(255, static_cast<int>(object.velocity.norm() * 1.5));
        color.g = 0;
        color.b = 0;
        color.a = 255;

        if (object.locked)
            color.b = 180;

        // std::cout << object.pos.x << "\t" << object.pos.y << std::endl;
        graphics.circle(object.pos.x, object.pos.y, object.r, color);
    }

    graphics.render();
}
