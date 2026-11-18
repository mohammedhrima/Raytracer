#include "render/Window.hpp"

#include <iostream>

Window::Window() : window(NULL), renderer(NULL), texture(NULL), width(0) {}

Window::~Window()
{
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::open(const char *title, int w, int h)
{
    width = w;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h, SDL_WINDOW_SHOWN);
    if (window)
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer)
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!texture)
    {
        std::cerr << "Cannot create window: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void Window::present(const uint32_t *pixels)
{
    SDL_UpdateTexture(texture, NULL, pixels, width * (int)sizeof(uint32_t));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Window::setTitle(const std::string &title)
{
    SDL_SetWindowTitle(window, title.c_str());
}
