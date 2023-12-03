#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <SDL2/SDL.h>
#include "Vector2.hpp"

class GameObject
{
    public:
        bool locked = false;
        Vector2 pos;
        Vector2 velocity;
        Vector2 force;
        Uint64 r=16;

        double kineticEnergy() { return 0.5f*velocity.normSquared(); };
};

#endif