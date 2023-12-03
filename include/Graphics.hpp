#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SDL2/SDL.h>

/* Graphics class
 * Creates a window and draws on it
 *
 * Idea: provide an abstraction layer, so that
 *       the rest of the code can be decoupled from SDL */

struct Color
{
    unsigned int r,g,b;
};

class Graphics
{
    public:
        Graphics();
        ~Graphics();

        void set_bg_color(Color color);
        void clear_window();
        void rectangle(int x, int y, int w, int h);
        void emptyCircle(int x_center, int y_center, int radius);
        void circle(int x_center, int y_center, int radius, Color color);
        void render();

        int get_width();
        int get_height();

        void handleWindowResize();

    // private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* window_surface;

        Color bg_color;
};

#endif