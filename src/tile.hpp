#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

#include <string>

#include <SDL/SDL.h>

class Tile
{
    std::string _filename;
    
    typedef unsigned char byte;
    
    byte * _data;
    size_t _width;
    size_t _height;
    
    void allocate(size_t w, size_t h);

public:

    Tile(std::string filename);
    ~Tile();
    
    bool load_png(std::string filename);
    bool load()
    {
        return load_png(_filename);
    }
    void clean_data();
    bool draw_to_SDL(SDL_Surface *surface);
    size_t inline width() const
    {
        return _width;
    }
    size_t inline height() const
    {
        return _height;
    }
    bool inline ready() const
    {
        return _data != NULL;
    }
};

#endif
