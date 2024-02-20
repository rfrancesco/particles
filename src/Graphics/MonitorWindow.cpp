#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "Graphics/MonitorWindow.hpp"
#include "SDLManager.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <implot.h>
#include <numeric>
#include <algorithm>
#include <exception>

PlotStream::PlotStream(int capacity) : maxCapacity{capacity}
{
    if (capacity <= 0)
        throw std::invalid_argument("Cannot create a PlotStream with zero or negative capacity!");
}

PlotStream &PlotStream::operator<<(double dataPoint)
{
    // std::cout << dataPoint << std::endl;
    ++currentIndex;
    if (currentIndex >= maxCapacity)
        currentIndex -= maxCapacity;
    if (data.size() < maxCapacity)
        data.push_back(dataPoint);
    else
        data[currentIndex] = dataPoint;
    return *this;
}

double PlotStream::mean() const
{
    return std::accumulate(data.begin(), data.end(), 0.) / data.size();
}

double PlotStream::min() const
{
    return *std::min_element(data.begin(), data.end());
}

double PlotStream::max() const
{
    return *std::max_element(data.begin(), data.end());
}

int PlotStream::size() const { return data.size(); }

static void if_true_crash(bool test, std::string msg)
{
    if (test)
    {
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

MonitorWindow::MonitorWindow(SimulationWindow& simulationWindow)
{
    SDLManager::initSDLIfNotYetInitialized();
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              600, 670, window_flags);
    if_true_crash(!window, "Could not create SDL Window!");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if_true_crash(!renderer, "Could not create SDL Renderer!");

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    int x, y, w, h;
    SDL_GetWindowPosition(simulationWindow.window, &x, &y);
    SDL_GetWindowSize(simulationWindow.window, &w, &h);
    SDL_SetWindowPosition(simulationWindow.window, x - w, y);
    SDL_SetWindowPosition(window, x, y);
}

MonitorWindow::~MonitorWindow()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void MonitorWindow::renderImGuiWindow_thermalization(unsigned int count, unsigned int tot)
{

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    ImVec2 vec{0, 0};
    ImGui::SetNextWindowPos(vec);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    vec = ImVec2{w, h};
    ImGui::SetNextWindowSize(vec);

    ImGui::Begin("Plots");
    ImGui::OpenPopup("Thermalizing");

    if (ImGui::BeginPopupModal("Thermalizing"))
    {
        ImGui::Text("Waiting for the system to settle down...");
        ImGui::ProgressBar(static_cast<double>(count) / tot);
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

static bool show_init_popup = true;
static bool model_is_locked = false;
static double model_locked_pa = 0.0;
void MonitorWindow::renderImGuiWindow_plots()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    ImVec2 vec{0, 0};
    ImGui::SetNextWindowPos(vec);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    vec = ImVec2{w, h};
    ImGui::SetNextWindowSize(vec);
    ImGui::Begin("Plots");

    double max = 0;
    double min = FLT_MAX;

    max = std::max(max, pressure.max());
    max = std::max(max, area.max());
    max = std::max(max, PA.max());

    min = std::min(min, pressure.min());
    min = std::min(min, area.min());
    min = std::min(min, PA.min());

    min *= 0.7;
    max *= 1.2;

    if (ImPlot::BeginPlot("Physical quantities"))
    {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, pressure.size(), ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, min, max, ImPlotCond_Always);
        std::vector<double> x(pressure.size());
        std::iota(x.begin(), x.end(), 1);
        ImPlot::SetupAxis(ImAxis_X1, "Time [frames]");
        ImPlot::SetupAxis(ImAxis_Y1, "[a.u.]");
        ImPlot::PlotLine("PA", x.data(), PA.gdata(), PA.size());
        // std::vector<double> smoothed = smooth(data[0],200);
        // ImPlot::PlotLine("PA (smoothed)", x.data(), smoothed.data(), smoothed.size());
        std::iota(x.begin(), x.end(), 1);
        ImPlot::PlotLine("P", x.data(), pressure.gdata(), pressure.size());
        if (model_is_locked)
        {
            std::vector<double> xmodel(2);
            xmodel[0] = x[0];
            xmodel[1] = x[x.size() - 1];
            std::vector<double> model(xmodel.size(), model_locked_pa);
            ImPlot::PlotLine("PA (locked)", xmodel.data(), model.data(), model.size());
        }
        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("P-A"))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Area");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, area.max() * 1.5, ImPlotCond_Always);
        ImPlot::SetupAxis(ImAxis_Y1, "Pressure");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, pressure.max() * 1.5, ImPlotCond_Always);
        ImPlot::PlotScatter("History", area.gdata(), pressure.gdata(), pressure.size());
        ImPlot::PlotScatter("You are here", area.plast(), pressure.plast(), 1);

        double model_PA;
        if (model_is_locked)
            model_PA = model_locked_pa;
        else
            model_PA = pressure.last() * area.last();
        std::vector<double> xx, yy;
        for (double x = 0.01; x < area.max() * 1.5; x += 0.01)
        {
            xx.push_back(x);
            yy.push_back(model_PA / x);
        }
        ImPlot::PlotLine("Model", xx.data(), yy.data(), yy.size());
        ImPlot::EndPlot();
    }

    if (!model_is_locked)
    {
        if (ImGui::Button("Lock model"))
        {
            model_is_locked = true;
            model_locked_pa = pressure.last() * area.last();
        }
    }
    else
    {
        if (ImGui::Button("Unlock model"))
            model_is_locked = false;
    }

    if (show_init_popup)
        ImGui::OpenPopup("Welcome");

    if (ImGui::BeginPopupModal("Welcome"))
    {
        ImGui::Text("This is a simulation of a 2D ideal gas.\n"
                    "Feel free to resize the main simulation window, to change its volume.\n"
                    "Notice that, at equilibrium, PA = const!\n\n");
        ImGui::Text("Remember that resizing the window brings the gas out of equilibrium.\n"
                    "After resizing, just wait a few seconds for the gas to settle down.");
        if (ImGui::Button("Ok!"))
        {
            show_init_popup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}
