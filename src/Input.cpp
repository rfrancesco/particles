#include "Input.hpp"
#include <iostream>

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