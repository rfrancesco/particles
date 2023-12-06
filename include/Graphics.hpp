#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SDL2/SDL.h>
#include <vector>

/* Graphics class
 * Creates a window and draws on it
 *
 * Idea: provide an abstraction layer, so that
 *       the rest of the code can be decoupled from SDL */

struct Color
{
    unsigned int r, g, b, a;
};

class Graphics
{
public:
    Graphics();
    ~Graphics();

    void set_bg_color(Color color);
    void clear_window();
    void rectangle(int x, int y, int w, int h);
    void circle(int x_center, int y_center, int radius, Color color);
    void render();

    void renderImGuiWindow_thermalization(unsigned int count, unsigned int tot);
    void renderImGuiWindow_plots(std::vector<std::vector<float>> &data);

    int get_width();
    int get_height();

    void handleWindowResize();

    // private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *window_surface;

    SDL_Window *info_window;
    SDL_Renderer *info_renderer;

    Color bg_color;
};

#endif
