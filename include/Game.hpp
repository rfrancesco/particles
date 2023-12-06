#ifndef GAME_HPP
#define GAME_HPP

#include "Graphics.hpp"
#include "GameObject.hpp"
#include <vector>

/* Game class
 * Contains the core logic
 *
 * */

class Game
{
public:
    Game();
    ~Game();

private:
    std::vector<GameObject> objects;

    void gameLoop();
    void draw(Graphics &graphics);

    const int FPS = 60;
};

#endif