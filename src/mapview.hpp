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

#ifndef MAPVIEW_HPP_INCLUDED
#define MAPVIEW_HPP_INCLUDED

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <SDL2pp/Font.hh>
#include <SDL2pp/Optional.hh>
#include <SDL2pp/Point.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "coord.hpp"
#include "tile_id.hpp"

namespace osmview
{

class TileCache;

class Mapview
{
    enum class VisitOrder
    {
        by_column,
        from_center,
    };

    static const int tile_size_ = 256;
    static const int max_level_ = 18;

    static const double v0_;
    static const double tau_;

    double mapx_;
    double mapy_;
    
    double vx_, vy_;
    double fx_, fy_;
    
    int target_level_;
    double level_;
    double scale_;
    
    std::unique_ptr<TileCache> cache_;

    SDL2pp::Renderer &renderer_;
    SDL2pp::Point output_size_;

    SDL2pp::Font font_;
    SDL2pp::Optional<SDL2pp::Texture> credits_texture_;

    SDL2pp::Point to_screen(double x, double y);
    std::pair<double, double> from_screen(const SDL2pp::Point &point);

    std::vector<std::pair<TileId, SDL2pp::Rect>>
    visible_tiles(int tile_level, VisitOrder order = VisitOrder::from_center);

public:

    explicit Mapview(SDL2pp::Renderer &renderer);
    ~Mapview();
    
    void center_on_latlon(double lat, double lon);
    void move(double move_x, double move_y);
    void move_pix_hard(double dx, double dy);
    void update(double dt);
    
    int zoom(int step);
    
    void render();
};

} // namespace

#endif
