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
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "sdl3.hpp"

namespace osmview
{

class Layer;

class Mapview
{
    static constexpr int tile_size_ = 256;
    static constexpr int max_level_ = 18;

    static constexpr float v0_ = 2.0f;
    static constexpr float tau_ = 0.3f;

    float mapx_;
    float mapy_;
    
    float vx_, vy_;
    float fx_, fy_;
    
    float target_level_;
    float level_;
    float scale_;
    
    std::vector<std::unique_ptr<Layer>> layers_;

    SDL3::Renderer &renderer_;
    SDL3::Point output_size_;
    SDL3::FPoint center_;

    SDL3::Font font_;
    std::optional<SDL3::Texture> credits_texture_;

    bool show_hud_;

    float current_fps_;
    float smoothed_fps_;

    SDL3::FPoint to_screen(float x, float y);
    std::pair<float, float> from_screen(const SDL3::FPoint &point);

    void render_hud();
    SDL3::Texture make_text_texture(const std::string & text, const SDL_Color & color);

public:

    explicit Mapview(SDL3::Renderer &renderer);
    ~Mapview();

    void center_on_latlon(osmview::point_latlon);
    void move(float move_x, float move_y);
    void move_pix_hard(float dx, float dy);
    void update(float dt);
    
    float zoom(float step);

    void toggle_hud()
    {
        show_hud_ = !show_hud_;
    }
    
    void render(const BudgetTimer& frame_timer);
};

} // namespace osmview

#endif
