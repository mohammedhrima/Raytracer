#include <iostream>
#include <map>
#include <sstream>

#include "render/Camera.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"
#include "scene/Scene.hpp"

namespace
{
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const float PI = 3.14159265358979f;
    const float CAMERA_STEP = 0.5f;
    const float OBJECT_STEP = 0.25f;

    struct Input
    {
        bool mouseDown;
        int lastX;
        int lastY;
        int selected;
        std::map<SDL_Keycode, Vec3> cameraKeys;
        std::map<SDL_Keycode, Vec3> objectKeys;

        Input() : mouseDown(false), lastX(0), lastY(0), selected(-1)
        {
            cameraKeys[SDLK_UP] = Vec3(0, CAMERA_STEP, 0);
            cameraKeys[SDLK_DOWN] = Vec3(0, -CAMERA_STEP, 0);
            cameraKeys[SDLK_LEFT] = Vec3(-CAMERA_STEP, 0, 0);
            cameraKeys[SDLK_RIGHT] = Vec3(CAMERA_STEP, 0, 0);
            cameraKeys[SDLK_PAGEUP] = Vec3(0, 0, -CAMERA_STEP);
            cameraKeys[SDLK_PAGEDOWN] = Vec3(0, 0, CAMERA_STEP);

            // Zoom along the view direction. '+' is Shift+'=' on most layouts.
            cameraKeys[SDLK_EQUALS] = Vec3(0, 0, -CAMERA_STEP);
            cameraKeys[SDLK_PLUS] = Vec3(0, 0, -CAMERA_STEP);
            cameraKeys[SDLK_KP_PLUS] = Vec3(0, 0, -CAMERA_STEP);
            cameraKeys[SDLK_MINUS] = Vec3(0, 0, CAMERA_STEP);
            cameraKeys[SDLK_KP_MINUS] = Vec3(0, 0, CAMERA_STEP);

            objectKeys[SDLK_KP_8] = Vec3(0, OBJECT_STEP, 0);
            objectKeys[SDLK_KP_2] = Vec3(0, -OBJECT_STEP, 0);
            objectKeys[SDLK_KP_4] = Vec3(-OBJECT_STEP, 0, 0);
            objectKeys[SDLK_KP_6] = Vec3(OBJECT_STEP, 0, 0);
            objectKeys[SDLK_KP_5] = Vec3(0, 0, -OBJECT_STEP);
            objectKeys[SDLK_KP_0] = Vec3(0, 0, OBJECT_STEP);
        }
    };

    bool handleKey(SDL_Keycode key, Input &input, Camera &camera, Scene &scene, bool &quit)
    {
        if (key == SDLK_ESCAPE)
        {
            quit = true;
            return false;
        }
        if (key == SDLK_r)
        {
            camera.reset();
            std::cout << "Camera reset" << std::endl;
            return true;
        }
        if (key >= SDLK_0 && key <= SDLK_9)
        {
            int index = key - SDLK_0;
            if (index < (int)scene.shapes.size())
            {
                input.selected = index;
                std::cout << "Selected " << scene.shapes[index]->name() << " " << index << std::endl;
            }
            else
                std::cout << "No object " << index << std::endl;
            return false;
        }

        std::map<SDL_Keycode, Vec3>::const_iterator it = input.cameraKeys.find(key);
        if (it != input.cameraKeys.end())
        {
            camera.move(it->second);
            return true;
        }

        it = input.objectKeys.find(key);
        if (it != input.objectKeys.end())
        {
            if (input.selected < 0)
            {
                std::cout << "Select an object first (keys 0-9)" << std::endl;
                return false;
            }
            scene.shapes[input.selected]->move(it->second);
            return true;
        }
        return false;
    }

    bool handleEvent(const SDL_Event &event, Input &input, Camera &camera, Scene &scene, bool &quit)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            return false;

        case SDL_KEYDOWN:
            return handleKey(event.key.keysym.sym, input, camera, scene, quit);

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                input.mouseDown = true;
                input.lastX = event.button.x;
                input.lastY = event.button.y;
            }
            return false;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                input.mouseDown = false;
            return false;

        case SDL_MOUSEMOTION:
        {
            if (!input.mouseDown)
                return false;
            float dx = (float)(event.motion.x - input.lastX) / (WIDTH * 0.5f);
            float dy = (float)(event.motion.y - input.lastY) / (HEIGHT * 0.5f);
            input.lastX = event.motion.x;
            input.lastY = event.motion.y;
            camera.rotate(-dx * PI * 0.5f, -dy * PI * 0.5f);
            return true;
        }

        case SDL_MOUSEWHEEL:
            camera.move(Vec3(0, 0, event.wheel.y > 0 ? -CAMERA_STEP : CAMERA_STEP));
            return true;

        default:
            return false;
        }
    }
}

int main()
{
    // Declaration order matters: the renderer uses the scene and the camera,
    // so it is destroyed (and its threads joined) before them.
    Window window;
    if (!window.open("Mini Raytracer", WIDTH, HEIGHT))
        return 1;

    Scene scene;
    scene.addDefaultObjects();
    Camera camera(WIDTH, HEIGHT);
    Renderer renderer(scene, camera, WIDTH, HEIGHT);
    Input input;

    bool quit = false;
    while (!quit)
    {
        bool changed = false;
        SDL_Event event;
        while (SDL_PollEvent(&event))
            changed |= handleEvent(event, input, camera, scene, quit);
        if (changed)
            renderer.reset();

        Uint32 start = SDL_GetTicks();
        renderer.renderFrame();
        Uint32 elapsed = SDL_GetTicks() - start;

        window.present(renderer.pixels());

        std::ostringstream title;
        title << elapsed << " ms | " << renderer.samples() << " samples";
        window.setTitle(title.str());
    }
    return 0;
}
