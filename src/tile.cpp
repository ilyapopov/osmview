#include "tile.hpp"

#include <cstdio>
#include <iostream>

#include <png.h>

Tile::Tile(std::string filename)
: _filename(filename), _data(NULL), _width(0), _height(0)
{
    load_png(_filename);
}

Tile::~Tile()
{
    if(_data != NULL)
        delete[] _data;
}

bool Tile::load_png(std::string filename)
{
    FILE * fp = fopen(filename.c_str(), "rb");
    
    if (!fp)
    {
        return false;
    }
    
    size_t num_to_check = 8;
    png_byte header[8];

    if(fread(header, 1, num_to_check, fp) != num_to_check)
    {
        fclose(fp);
        return false;
    }
    
    bool is_png = !png_sig_cmp(header, 0, num_to_check);
    
    if (!is_png)
    {
        fclose(fp);
        return false;
    }
    
    png_structp png = png_create_read_struct
        (PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
        NULL, NULL);

    if (!png)
    {
		fclose (fp);
        return false;
    }
        
    png_infop info = png_create_info_struct(png);
    png_infop end_info = png_create_info_struct(png);
    
    if (!info || !end_info)
    {
        png_destroy_read_struct(&png,
            (png_infopp)info, (png_infopp)end_info);
		fclose (fp);
        return false;
    }
    
	if (setjmp (png_jmpbuf(png))) {
		png_destroy_read_struct (&png, &info, &end_info);
		fclose (fp);
		return false;
    }
    
    png_init_io(png, fp);

    png_set_sig_bytes(png, num_to_check);

    png_read_info(png, info);

	if (info->color_type == PNG_COLOR_TYPE_PALETTE)
	    png_set_palette_to_rgb(png);
	
	if (info->color_type == PNG_COLOR_TYPE_GRAY 
	    || info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
	    png_set_tRNS_to_alpha(png);

	if (info->interlace_type != PNG_INTERLACE_NONE) {
    	  png_destroy_read_struct (&png, &info, &end_info);
		  fclose (fp);
          return false;
    }

    if (info->color_type & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png);
        
    png_read_update_info(png, info);
    
    if(info->bit_depth != 8 || info->color_type != PNG_COLOR_TYPE_RGB)
    {
        std::cerr << "Unsupported color type or bit depth" << std::endl;
		png_destroy_read_struct (&png, &info, &end_info);
		fclose (fp);
		return false;
    }
    
    _width = info->width;
    _height = info->height;
    allocate(_width, _height);
    
    size_t row_size = 3 * _width;
    byte * row = new byte[row_size];
    
    for(size_t i = 0; i < _height; ++i)
    {
        png_read_row(png, reinterpret_cast<png_byte *>(row), NULL);
        memcpy(_data + 3 * _width * i, row, row_size);
    }
    
    delete[] row;

    png_read_end(png, end_info);
        
    png_destroy_read_struct(&png, &info, &end_info);
    
    fclose(fp);

    return true;
}

void Tile::allocate(size_t w, size_t h)
{
    if(_data != NULL)
        delete[] _data;
        
    _data = new byte[3 * w * h];
}

void Tile::clean_data()
{
    if(_data != NULL)
    {
        delete[] _data;
        _data = NULL;
    }
        
    _width = 0;
    _height = 0;
}

bool Tile::draw_to_SDL(SDL_Surface *surface)
{
    if(_data == NULL)
    {
        return false;
    }

    size_t w = std::min(_width, static_cast<size_t>(surface->w));
    size_t h = std::min(_height, static_cast<size_t>(surface->h));
    
    if ( SDL_MUSTLOCK(surface) ) {
        if ( SDL_LockSurface(surface) < 0 ) {
            return false;
        }
    }
    
    for(size_t j = 0; j < h; ++j)
    {
        for(size_t i = 0; i < w; ++i)
        {
            byte * q = _data + j*_width*3 + 3*i;
            
            Uint32 color = SDL_MapRGB(surface->format, *q, *(q+1), *(q+2));
            Uint32 * p = (Uint32 *)surface->pixels + j*surface->pitch/4 + i;
            *p = color;
        }
    }

    if ( SDL_MUSTLOCK(surface) ) {
        SDL_UnlockSurface(surface);
    }

    SDL_UpdateRect(surface, 0, 0, w, h);

    return true;
}
