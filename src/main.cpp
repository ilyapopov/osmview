#include <iostream>

#include <SDL/SDL.h>

#include "mapview.hpp"
#include "timer.hpp"

int main(int argc, char ** argv)
{
    std::cout << "Starting..." << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    SDL_Surface *screen;

    screen = SDL_SetVideoMode(1024, 768, 32, SDL_SWSURFACE);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        exit(1);
    }
    
    SDL_WM_SetCaption("OSMview", NULL);
    
    Mapview mv(screen);
    Timer motion_timer;

    while(1)
    {
        SDL_Delay(20);

        // 1. Process events
    
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                std::cout << "Exiting..." << std::endl;
                return 0;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_MINUS)
                {
                    mv.zoom(-1);
                }
                if(event.key.keysym.sym == SDLK_EQUALS)
                {
                    mv.zoom(1);
                }
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
        
        //Timer t1;
        
        mv.render(screen);
        
        //double tr = t1.delta();
        
        SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
        
        //std::cout << "F " << tr << '\t' << t1.delta() << std::endl;
        //std::cout << std::endl;
    
    }
    
    return 0;
}
