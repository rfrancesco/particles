#include "SDLManager.hpp"
#include <SDL2/SDL.h>
#include <iostream>

SDLManager *SDLManager::singleton = nullptr;

SDLManager::SDLManager()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "SDLManager: Cannot initialize SDL2!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

SDLManager::~SDLManager()
{
    SDL_Quit();
}

void SDLManager::initSDLIfNotYetInitialized()
{
    if (singleton == nullptr)
    {
        singleton = new SDLManager();
    }
}

SDLManager *SDLManager::getInstance()
{
    initSDLIfNotYetInitialized();
    return singleton;
}