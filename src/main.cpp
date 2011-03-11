#include "main.hpp"

#include <iostream>

#include "mapview.hpp"
#include "timer.hpp"

int main(int argc, char ** argv)
{
    std::cout << "Starting..." << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    SDL_WM_SetCaption("OSMview", NULL);
    
    SDL_Surface *screen;
    
    screen = set_video_mode(1024, 768);
    
    Mapview mv;
    Timer motion_timer;

    while(1)
    {
        SDL_Delay(20);

        // 1. Process events
    
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    std::cout << "Exiting..." << std::endl;
                    return 0;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_MINUS:
                            mv.zoom(-1);
                            break;
                        case SDLK_EQUALS:
                            mv.zoom(1);
                            break;
                        case SDLK_F11:
                            //SDL_WM_ToggleFullScreen(screen);
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_VIDEORESIZE:
                    screen = set_video_mode(event.resize.w, event.resize.h);
                    break;
                case SDL_VIDEOEXPOSE:
                    //nothing
                    break;
                default:
                    break;
            }
        }
        
        // 2. Process key states
        
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if(keystate[SDLK_LEFT])
        {
                    mv.move(0, -1);
        }
        if(keystate[SDLK_RIGHT])
        {
                    mv.move(0, 1);
        }
        if(keystate[SDLK_UP])
        {
                    mv.move(1, 0);
        }
        if(keystate[SDLK_DOWN])
        {
                    mv.move(-1, 0);
        }
        
        // 3. Update system state
        
        mv.motion_step(motion_timer.delta());
        
        // 4. Render the screen
        
        if(!(SDL_GetAppState() & SDL_APPACTIVE))
            continue;
        
        mv.render(screen);
        
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    
    }
    
    return 0;
}

SDL_Surface * set_video_mode(int w, int h)
{
    SDL_Surface * screen = SDL_SetVideoMode(w, h, 32, SDL_RESIZABLE);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        exit(1);
    }
    return screen;
}

