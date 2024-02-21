#include <iostream>
#include "Game.hpp"
#include "Graphics/SimulationWindow.hpp"
#include "Graphics/MonitorWindow.hpp"
#include "Input.hpp"
#include "GameLoopTimer.hpp"
#include "DataSmoother.hpp"

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
    SimulationWindow simulationGraphics;
    MonitorWindow monitorGraphics{simulationGraphics};
    Input input;
    bool running = true;
    GameLoopTimer frametimer;
    PhysicsEngine physics;
    double dt;
    unsigned long int count = 0;
    DataSmoother p{250}, a{250}, pa{250}, ke{250};

    for (int i = 0; i < 500; i++)
    {
        objects.push_back(GameObject());
        objects[i].pos = {rand() % simulationGraphics.getWidth(), rand() % simulationGraphics.getHeight()};
        objects[i].velocity = {rand() % 300 - 150, rand() % 300 - 150};
        objects[i].force = {0.0, 0.0};
        objects[i].r = 4;
    }

    (void)frametimer.getPhysicsDeltaTime();
    while (running)
    {
        count++;
        frametimer.start();
        input.pollInputEvents();
        running = !(input.wasQuitRequested());
        if (input.wasWindowResized())
            physics.setWorldBox(simulationGraphics.getWidth(), simulationGraphics.getHeight());

        dt = frametimer.getPhysicsDeltaTime();
        physics.computeForces(objects);
        physics.evolveObjects(objects, dt);
        frametimer.end();
        // std::cout << "Time for logic:" << frametimer.elapsed_time() << std::endl;

        if (monitorGraphics.status == MonitorWindowStatus::Thermalization)
        {
            monitorGraphics.renderImGuiWindow_thermalization(count, 200);
            if (count == 200)
                monitorGraphics.status = MonitorWindowStatus::Plot;
        }
        else
        {
            double pressure = physics.getPressure() / 1e2;
            double area = physics.getArea() / 1e5;
            p << pressure;
            a << area;
            pa << pressure * area;
            ke << physics.getTotalKineticEnergy(objects);

            pressure = p.mean();
            double pressurearea = pa.mean();
            double kinetic = ke.mean();
            area = a.mean();

            monitorGraphics.PA << pressurearea;
            monitorGraphics.pressure << pressure;
            monitorGraphics.area << area;

            monitorGraphics.renderImGuiWindow_plots();
        }

        draw(simulationGraphics);

        frametimer.end();
        // std::cout << "Time for logic+render:" << frametimer.elapsed_time() << std::endl;
        frametimer.capFPS(FPS);
        frametimer.logFPS();
    }
}

void Game::draw(SimulationWindow &simulationGraphics)
{
    simulationGraphics.clearWindow();

    // std::cout << "Objects" << std::endl;
    for (auto &object : objects)
    {
        Color color;
        color.r = std::min(255, static_cast<int>(object.velocity.norm() * 1.5));
        color.g = 0;
        color.b = 0;
        color.a = 255;

        // std::cout << object.pos.x << "\t" << object.pos.y << std::endl;
        simulationGraphics.circle(object.pos.x, object.pos.y, object.r, color);
    }

    simulationGraphics.render();
}
