#include "Input.hpp"
#include "SDLManager.hpp"
#include <iostream>

Input::Input()
    : quitStatus{false}, windowResizeStatus{false}
{
    SDLManager::initSDLIfNotYetInitialized();
}

void Input::pollInputEvents()
{
    SDL_Event event;
    beginNewFrame();
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_KEYDOWN)
            keyDownEvent(event);
        else if (event.type == SDL_KEYUP)
            keyUpEvent(event);
        if ((event.type == SDL_QUIT) || wasKeyPressed(SDL_SCANCODE_ESCAPE))
            quitStatus = true;
        else if (event.type == SDL_WINDOWEVENT)
            windowResizeStatus = true;
    }
}

bool Input::wasQuitRequested()
{
    return quitStatus;
}

bool Input::wasWindowResized()
{
    return windowResizeStatus;
}

Vector2 Input::getWASD()
{
    Vector2 delta = {0,0};
    if (isKeyHeld(SDL_SCANCODE_W))
        delta.y -= 1;
    else if (isKeyHeld(SDL_SCANCODE_S))
        delta.y += 1;
    if (isKeyHeld(SDL_SCANCODE_D))
        delta.x += 1;
    else if (isKeyHeld(SDL_SCANCODE_A))
        delta.x -= 1;

    return delta;
}

bool Input::isWASDHeld()
{
    return (isKeyHeld(SDL_SCANCODE_W) 
            || isKeyHeld(SDL_SCANCODE_A) 
            || isKeyHeld(SDL_SCANCODE_S) 
            || isKeyHeld(SDL_SCANCODE_D));
}

void Input::beginNewFrame()
{
    pressedKeys.clear();
    releasedKeys.clear();
}

void Input::keyDownEvent(const SDL_Event& event)
{
    if (!event.key.repeat)
    {
        pressedKeys[event.key.keysym.scancode] = true;
        heldKeys[event.key.keysym.scancode] = true;
        std::cout << "Pressed key!" << std::endl;
    }
}

void Input::keyUpEvent(const SDL_Event& event)
{
    releasedKeys[event.key.keysym.scancode] = true;
    heldKeys[event.key.keysym.scancode] = false;

    std::cout << "Released key!" << std::endl;
}

bool Input::wasKeyPressed(SDL_Scancode key)
{
    if (pressedKeys.count(key))
        return pressedKeys.at(key);
    else
        return false;
}

bool Input::wasKeyReleased(SDL_Scancode key)
{
    if (releasedKeys.count(key))
        return releasedKeys.at(key);
    else
        return false;
}


bool Input::isKeyHeld(SDL_Scancode key)
{
    if (heldKeys.count(key))
        return heldKeys.at(key);
    else
        return false;
}