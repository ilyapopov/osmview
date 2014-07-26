/*
    Copyright 2011, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.

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

#include <cstdlib>
#include <iostream>

#include <curl/curl.h>
#include "SDL2/SDL.h"

#include "mapview.hpp"
#include "timer.hpp"

int main(int argc, char ** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);
    
    curl_global_init(CURL_GLOBAL_ALL);
    atexit(curl_global_cleanup);
    
    SDL_Window *sdlWindow;
    SDL_Renderer *sdlRenderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &sdlWindow, &sdlRenderer);

    if (sdlRenderer == nullptr || sdlWindow == nullptr)
    {
        std::cerr << "FATAL: Cannot create window or renderer" << std::endl;
        return 1;
    }

    Mapview mv(sdlRenderer);
    Timer motion_timer;
    
    bool mouse_pan = false;
    int mousex = 0, mousey = 0;

    while(1)
    {
        SDL_Delay(10);

        // 1. Process events
    
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_MINUS:
                    mv.zoom(-1);
                    break;
                case SDL_SCANCODE_EQUALS:
                    mv.zoom(1);
                    break;
                case SDL_SCANCODE_F11:
                    //SDL_WM_ToggleFullScreen(screen);
                    break;
                default:
                    break;
                }
                break;
            case SDL_WINDOWEVENT_RESIZED:
                break;
            case SDL_MOUSEMOTION:
                //mv.move_pix_hard(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch(event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    mouse_pan = true;
                    SDL_GetRelativeMouseState(&mousex, &mousey);
                default:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch(event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    mouse_pan = false;
                default:
                    break;
                }
                break;
            case SDL_MOUSEWHEEL:
                mv.zoom(event.wheel.y);
                break;
            default:
                break;
            }
        }
        
        // 2. Process key states
        
        const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
        if(keystate[SDL_SCANCODE_LEFT])
        {
            mv.move(-1, 0);
        }
        if(keystate[SDL_SCANCODE_RIGHT])
        {
            mv.move(1, 0);
        }
        if(keystate[SDL_SCANCODE_UP])
        {
            mv.move(0, -1);
        }
        if(keystate[SDL_SCANCODE_DOWN])
        {
            mv.move(0, 1);
        }
        
        if(mouse_pan)
        {
            SDL_GetRelativeMouseState(&mousex, &mousey);
            mv.move_pix_hard(-mousex, -mousey);
        }
        
        // 3. Update system state
        
        mv.motion_step(motion_timer.delta());
        
        // 4. Render the screen
        
        mv.render();
        
        SDL_RenderPresent(sdlRenderer);
    }
    
    return 0;
}
