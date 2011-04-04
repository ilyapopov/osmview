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

#include "tilecacheitem.hpp"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>

#include <curl/curl.h>
#include <SDL/SDL_image.h>

TileCacheItem::TileCacheItem(const std::string file_name, const std::string url)
: _file_name(file_name), _url(url), _fetch_error(false), _surface(NULL)
{
}
    
TileCacheItem::~TileCacheItem()
{
    if(_surface != NULL)
        SDL_FreeSurface(_surface);
}

bool TileCacheItem::fetch()
{
    if(_surface != NULL)
        return true;
    SDL_Surface * s = IMG_Load(_file_name.c_str());
    if(s == NULL)
    {
        _fetch_error = true;
        return false;
    }
    _surface = SDL_DisplayFormat(s);
    return true;
}

bool TileCacheItem::download()
{
    if(_fetch_error == false)
        return true;
    _fetch_error = false;
    std::cout << "Downloading " << _url << " to " << _file_name << std::endl;
    
    boost::filesystem::path path(_file_name);
    
    boost::filesystem::create_directories(path.parent_path());

    CURL * curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    FILE * file = fopen(path.string().c_str(), "wb");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(file);

//    std::stringstream command;
//    command << "cd /home/ipopov/.cache/maps/tile.openstreetmap.org";
//    command << " && ";
//    command << "wget -x -nH \"" << _url << "\"";

//    system(command.str().c_str());
    
    std::cout << "Done." << std::endl;
    
    return true;
}

