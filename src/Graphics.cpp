#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "Graphics.hpp"
#include "SDLManager.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <implot.h>
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

Graphics::Graphics()
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
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "2");
    clear_window();
    render();

    info_window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   600, 650, window_flags);
    if_true_crash(!info_window, "Could not create SDL Window!");
    info_renderer = SDL_CreateRenderer(info_window, -1, SDL_RENDERER_ACCELERATED);
    if_true_crash(!info_renderer, "Could not create SDL Renderer!");

    int x, y, w, h;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowSize(window, &w, &h);
    SDL_SetWindowPosition(window, x - w, y);
    SDL_SetWindowPosition(info_window, x, y);

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(info_window, info_renderer);
    ImGui_ImplSDLRenderer2_Init(info_renderer);
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

// static std::vector<float> smooth(const std::vector<float> &in, unsigned int n)
// {
//     std::vector<float> out(in.size());
//     for (size_t i = 0; i < in.size(); ++i)
//     {
//         size_t r = (i + n) < in.size() ? i + n : in.size() - 1;
//         size_t l = ((i - n) < r) ? (i - n) : 0; // size_t is unsigned!
//         size_t diff = r - l;

//         float avg = 0.;
//         for (; l < r; ++l)
//             avg += in[r];

//         out[i] = avg / diff;
//     }
//     std::cout << out[out.size() - 1] << std::endl;
//     return out;
// }

void Graphics::renderImGuiWindow_thermalization(unsigned int count, unsigned int tot)
{

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("Plots");
    ImGui::OpenPopup("Thermalizing");

    if (ImGui::BeginPopupModal("Thermalizing"))
    {
        ImGui::Text("Waiting for the system to settle down...");
        ImGui::ProgressBar(static_cast<float>(count) / tot);
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(info_renderer);
}

static bool show_init_popup = true;
void Graphics::renderImGuiWindow_plots(std::vector<std::vector<float>> &data)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    ImVec2 vec{0, 0};
    ImGui::SetNextWindowPos(vec);
    int w, h;
    SDL_GetWindowSize(info_window, &w, &h);
    vec = ImVec2{w, h};
    ImGui::SetNextWindowSize(vec);
    ImGui::Begin("Plots");

    if (ImPlot::BeginPlot("Physical quantities"))
    {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, data[0].size(), ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 2, ImPlotCond_Always);
        std::vector<float> x(data[0].size());
        std::iota(x.begin(), x.end(), 1);
        ImPlot::SetupAxis(ImAxis_X1, "Time [frames]");
        ImPlot::SetupAxis(ImAxis_Y1, "[a.u.]");
        ImPlot::PlotLine("PA", x.data(), data[0].data(), data[0].size());
        // std::vector<float> smoothed = smooth(data[0],200);
        // ImPlot::PlotLine("PA (smoothed)", x.data(), smoothed.data(), smoothed.size());
        std::iota(x.begin(), x.end(), 1);
        ImPlot::PlotLine("P", x.data(), data[1].data(), data[1].size());
        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("P-A"))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Area");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, *std::max_element(data[2].begin(), data[2].end()) * 2., ImPlotCond_Always);
        ImPlot::SetupAxis(ImAxis_Y1, "Pressure");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, *std::max_element(data[1].begin(), data[1].end()) * 2., ImPlotCond_Always);
        ImPlot::PlotScatter("History", data[2].data(), data[1].data(), data[1].size());
        ImPlot::PlotScatter("You are here", data[2].data() + data[2].size() - 1, data[1].data() + data[1].size() - 1, 1);
        float current_PA = data[2][data[2].size() - 1] * data[1][data[1].size() - 1];
        std::vector<float> xx, yy;
        for (float x = 0.1; x < *std::max_element(data[2].begin(), data[2].end()) * 2.; x += 0.01)
        {
            xx.push_back(x);
            yy.push_back(current_PA / x);
        }
        ImPlot::PlotLine("Model", xx.data(), yy.data(), yy.size());
        ImPlot::EndPlot();
    }

    ImGui::End();

    if (show_init_popup)
        ImGui::OpenPopup("Welcome");

    if (ImGui::BeginPopupModal("Welcome"))
    {
        ImGui::Text("This is a simulation of a 2D ideal gas.\n"
                    "Feel free to resize the main simulation window, to change its volume.\n"
                    "Notice that, at equilibrium, PA = const!\n\n");
        ImGui::Text("Remember that resizing the window brings the system out of equilibrium.\n"
                    "After resizing, just wait a few seconds for the system to settle down.");
        if (ImGui::Button("Ok!"))
        {
            show_init_popup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(info_renderer);
}

int Graphics::get_width()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return w;
}

int Graphics::get_height()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return h;
}

void Graphics::handleWindowResize()
{
    SDL_FreeSurface(window_surface);
    window_surface = SDL_GetWindowSurface(window);
}

void Graphics::circle(int x_center, int y_center, int radius, Color color)
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
