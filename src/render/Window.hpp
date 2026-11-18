#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL.h>
#include <stdint.h>
#include <string>

class Window
{
public:
    Window();
    ~Window();

    bool open(const char *title, int width, int height);
    void present(const uint32_t *pixels);
    void setTitle(const std::string &title);

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width;

    Window(const Window &);
    Window &operator=(const Window &);
};

#endif
