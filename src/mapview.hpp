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

#include "coord.hpp"
#include "timer.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "SDL.h"
#include "SDL2pp/Font.hh"
#include "SDL2pp/Optional.hh"
#include "SDL2pp/Point.hh"
#include "SDL2pp/Texture.hh"

namespace SDL2pp
{
    class Renderer;
} // namespace SDL2pp

namespace osmview
{

class Layer;

class Mapview
{
    static constexpr int tile_size_ = 256;
    static constexpr int max_level_ = 18;

    static constexpr double v0_ = 2.0;
    static constexpr double tau_ = 0.3;

    double mapx_;
    double mapy_;
    
    double vx_, vy_;
    double fx_, fy_;
    
    int target_level_;
    double level_;
    double scale_;
    
    std::vector<std::unique_ptr<Layer>> layers_;

    SDL2pp::Renderer &renderer_;
    SDL2pp::Point output_size_;

    SDL2pp::Font font_;
    SDL2pp::Optional<SDL2pp::Texture> credits_texture_;

    bool show_hud_;

    double current_fps_;
    double smoothed_fps_;

    SDL2pp::Point to_screen(double x, double y);
    std::pair<double, double> from_screen(const SDL2pp::Point &point);

    void render_hud();
    SDL2pp::Texture make_text_texture(const std::string & text, const SDL_Color & color);

public:

    explicit Mapview(SDL2pp::Renderer &renderer);
    ~Mapview();

    void center_on_latlon(osmview::point_latlon);
    void move(double move_x, double move_y);
    void move_pix_hard(double dx, double dy);
    void update(double dt);
    
    int zoom(int step);

    void toggle_hud()
    {
        show_hud_ = !show_hud_;
    }
    
    void render(const BudgetTimer& frame_timer);
};

} // namespace osmview

#endif
