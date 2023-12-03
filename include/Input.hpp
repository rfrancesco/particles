#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL2/SDL.h>
#include <unordered_map>

/* Input class
 * Allows keeping track of keyboard events
 *
 * TODO: provide an interface to Game, that
 * abstracts SDL as much as possible.
 * 
 * Examples: provide
 * - handleInputEvents(); (-> SDL_PollEvents, etc.)
 * - getMovementWASD() 
 * etc. */

class Input
{
    public:
        void beginNewFrame();
        void keyUpEvent(const SDL_Event& event);
        void keyDownEvent(const SDL_Event& event);

        bool wasKeyPressed(SDL_Scancode key);
        bool wasKeyReleased(SDL_Scancode key);
        bool isKeyHeld(SDL_Scancode key);

    private:
        std::unordered_map<SDL_Scancode, bool> heldKeys;
        std::unordered_map<SDL_Scancode, bool> pressedKeys;
        std::unordered_map<SDL_Scancode, bool> releasedKeys;      
};

#endif