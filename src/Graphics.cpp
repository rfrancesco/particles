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
    window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                    300,300,window_flags);
    if_true_crash(!window, "Could not create SDL Window!");
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if_true_crash(!renderer, "Could not create SDL Renderer!");
    window_surface = SDL_GetWindowSurface(window);
    if_true_crash(!window_surface,"Could not get SDL Surface from main window");
    set_bg_color({255,201,211,255});
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2"  );
    SDL_SetHint( SDL_HINT_RENDER_VSYNC, "2"  );
    clear_window();
    render();

    info_window = SDL_CreateWindow("Particles!", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                    600,400,window_flags);
    if_true_crash(!info_window, "Could not create SDL Window!");
    info_renderer = SDL_CreateRenderer(info_window,-1,SDL_RENDERER_ACCELERATED);
    if_true_crash(!info_renderer, "Could not create SDL Renderer!");

    int x, y, w, h;
    SDL_GetWindowPosition(window,&x,&y);
    SDL_GetWindowSize(window, &w, &h);
    SDL_SetWindowPosition(window, x-w, y);
    SDL_SetWindowPosition(info_window, x, y);

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

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

static std::vector<float> smooth(const std::vector<float> &in, unsigned int n)
{
    std::vector<float> out(in.size());
    for (size_t i=0; i<in.size(); ++i)
    {
        size_t r = (i+n) < in.size() ? i+n : in.size()-1;
        size_t l = ((i-n) < r) ? (i-n) : 0;    // size_t is unsigned!
        size_t diff = r-l;

        float avg = 0.;
        for (;l<r;++l)
            avg += in[r];
        
        out[i] = avg / diff;
    }
    std::cout << out[out.size()-1] <<std::endl;
    return out;
}

static bool init=true;
void Graphics::renderImGuiWindow(std::vector<std::vector<float>> &data)
{
    /* TODO: It would be nice to be able to "stream" data into a plot... */
    ImGuiIO& io = ImGui::GetIO(); 
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    ImVec2 vec {0,0};
    ImGui::SetNextWindowPos(vec);
    int w, h;
    SDL_GetWindowSize(info_window,&w, &h);
    vec = ImVec2{w,h};
    ImGui::SetNextWindowSize(vec);
    ImGui::Begin("Plots");                        // Create a window called "Hello, world!" and append into it.
    

    if (ImPlot::BeginPlot("Physical quantities")) {
       
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, data[0].size(), ImPlotCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
            std::vector<float> x(data[0].size());
            std::iota(x.begin(), x.end(), 1);
            ImPlot::PlotLine("PA", x.data(), data[0].data(), data[0].size());
            std::vector<float> smoothed = smooth(data[0],200);
            ImPlot::PlotLine("PA (smoothed)", x.data(), smoothed.data(), smoothed.size());
            std::iota(x.begin(), x.end(), 1);
            ImPlot::PlotLine("P", x.data(), data[1].data(), data[1].size());
        ImPlot::EndPlot();
    }

    ImGui::End();

    if (init)
        ImGui::OpenPopup("Welcome");
    
    if (ImGui::BeginPopupModal("Welcome"))
    {
        std::cout << init << std::endl;
        ImGui::Text("This is a simulation of a 2D ideal gas.\n"
                    "Feel free to resize the main simulation window, to change its volume.\n"
                    "Notice that (roughly) PA = const!\n\n"
                    "Remember to let the system settle for a little while after resizing.\n"
                    "Small volumes can lead to deviations from the ideal gas law.");
        if (ImGui::Button("Ok!"))
        {
            init = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Render();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // SDL_RenderSetScale(info_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    // SDL_SetRenderDrawColor(info_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(info_renderer);
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

void Graphics::circle(int x_center, int y_center, int radius, Color color)
{
    if (radius<=0)
        return;

    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

    std::vector<SDL_Point> points;
    for (int x = 0; x < radius * 2; x++)
    {
        for (int y = 0; y < radius * 2; y++)
        {
            int dx = radius - x; 
            int dy = radius - y; 
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                points.push_back({x_center+dx,y_center+dy});
            }
        }
    }
    SDL_RenderDrawPoints( renderer, points.data(), points.size() );
}
