/*
    Copyright 2011, 2014, 2015, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.
    https://bitbucket.org/ipopov/osmview

    osmview is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    osmview is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with osmview.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mapview.hpp"
#include "timer.hpp"

#include "sdl3.hpp"
#include <SDL3/SDL_events.h>

#include <exception>
#include <iostream>

int main(int /*argc*/, char ** /*argv*/)
{
    try
    {
        SDL3::SDL sdl(SDL_INIT_VIDEO);
        SDL3::SDLTTF sdlttf;

        SDL3::Window window("OpenStreetMap viewer",
                              800, 600,
                              SDL_WINDOW_RESIZABLE
                              );

        bool fullscreen = false;

        SDL3::Renderer renderer(window);
        renderer.SetVSync(1);

        osmview::Mapview mv(renderer);
        osmview::DeltaTimer motion_timer;

        bool mouse_pan = false;
        float mousex = 0;
        float mousey = 0;

        while (true)
        {
            osmview::BudgetTimer frame_timer(std::chrono::milliseconds(12));
            // 1. Process events
            SDL_Event event{};
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    return 0;
                case SDL_EVENT_KEY_DOWN:
                    switch (event.key.scancode)
                    {
                    case SDL_SCANCODE_MINUS:
                        mv.zoom(-1);
                        break;
                    case SDL_SCANCODE_EQUALS:
                        mv.zoom(1);
                        break;
                    case SDL_SCANCODE_F11:
                        window.SetFullscreen(!fullscreen);
                        fullscreen = !fullscreen;
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        return 0;
                    case SDL_SCANCODE_TAB:
                        mv.toggle_hud();
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    switch (event.button.button)
                    {
                    case SDL_BUTTON_LEFT:
                        mouse_pan = true;
                        SDL_GetRelativeMouseState(&mousex, &mousey);
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    switch (event.button.button)
                    {
                    case SDL_BUTTON_LEFT:
                        mouse_pan = false;
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    mv.zoom(event.wheel.y);
                    break;
                default:
                    break;
                }
            }

            // 2. Process key states

            const bool *keystate = SDL_GetKeyboardState(nullptr);
            if (keystate[SDL_SCANCODE_LEFT])
            {
                mv.move(-1, 0);
            }
            if (keystate[SDL_SCANCODE_RIGHT])
            {
                mv.move(1, 0);
            }
            if (keystate[SDL_SCANCODE_UP])
            {
                mv.move(0, -1);
            }
            if (keystate[SDL_SCANCODE_DOWN])
            {
                mv.move(0, 1);
            }

            if (mouse_pan)
            {
                SDL_GetRelativeMouseState(&mousex, &mousey);
                mv.move_pix_hard(-mousex, -mousey);
            }

            // 3. Update system state

            mv.update(motion_timer.delta());

            // 4. Render the screen

            renderer.Clear();

            mv.render(frame_timer);

            renderer.Present();
        }
    }
    catch (std::exception & e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error" << std::endl;
        return 2;
    }
}
