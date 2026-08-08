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

#include "coord.hpp"
#include "filesystem.hpp"
#include "layer.hpp"
#include "tilelayer.hpp"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "sdl3.hpp"

SDL3::FPoint osmview::Mapview::to_screen(float x, float y)
{
    float x1 = (x - mapx_) * scale_;
    float y1 = (y - mapy_) * scale_;
    return SDL3::FPoint{x1, y1} + center_;
}

std::pair<float, float> osmview::Mapview::from_screen(const SDL3::FPoint &point)
{
    SDL3::FPoint p1 = point - center_;
    return {mapx_ + p1.x / scale_, mapy_ + p1.y / scale_};
}

osmview::Mapview::Mapview(SDL3::Renderer &renderer)
    : mapx_(0.5), mapy_(0.5),
    vx_(0.0), vy_(0.0), fx_(0.0), fy_(0.0),
    target_level_(1), level_(1.0), scale_(1.0),
    renderer_(renderer), output_size_(renderer_.GetOutputSize()),
    center_{float(output_size_.x/2), float(output_size_.y/2)},
    font_("data/DejaVuSans.ttf", 12),
    show_hud_(false),
    current_fps_(0), smoothed_fps_(0)
{
    std::string server_name("tile.openstreetmap.org");

    layers_.emplace_back(std::make_unique<TileLayer>(server_name,
                                                     "http://" + server_name,
                                                     get_user_cache_dir() / "maps" / server_name,
                                                     256,
                                                     renderer
                                                     ));

    layers_.emplace_back(std::make_unique<TileLayer>("OpenSeaMap",
                                                     "http://tiles.openseamap.org/seamark",
                                                     get_user_cache_dir() / "maps" / "tiles.openseamap.org",
                                                     256,
                                                     renderer
                                                     ));

    SDL3::Surface credits_surface = font_.RenderText_Shaded(
        "Cartography © OpenStreetMap contributors | CC BY-SA | openstreetmap.org",
        {64, 64, 64, 0}, {255, 255, 255, 0});
    credits_texture_.emplace(SDL3::Texture(renderer_, credits_surface));
}

osmview::Mapview::~Mapview() = default;

void osmview::Mapview::center_on_latlon(point_latlon latlon)
{
    point_xy xy = latlon2xy(latlon);
    mapx_ = xy.x;
    mapy_ = xy.y;
    vx_ = vy_ = 0.0;
    fx_ = fy_ = 0.0;
}

void osmview::Mapview::move(float move_x, float move_y)
{
    fx_ += v0_ * move_x / scale_;
    fy_ += v0_ * move_y / scale_;
}

void osmview::Mapview::move_pix_hard(float dx, float dy)
{
    mapx_ += dx / tile_size_ / scale_;
    mapy_ += dy / tile_size_ / scale_;
}

float osmview::Mapview::zoom(float step)
{
    target_level_ = clamp(target_level_ + step, 0.0f, float(max_level_));

    //TODO(ipopov):
    //for (const auto & p: visible_tiles(target_level_, VisitOrder::from_center))
    //{
    //    cache_->prefetch(p.first);
    //}

    return target_level_;
}

void osmview::Mapview::render(const BudgetTimer& frame_timer)
{
    for (const auto &l: layers_)
    {
        l->render(level_, {mapx_, mapy_}, renderer_, frame_timer);
    }

    if (show_hud_)
    {
        render_hud();
    }
}

void osmview::Mapview::update(float dt)
{
    float beta = 1.0f - std::exp(-dt / tau_);

    vx_ += (fx_ - vx_) * beta;
    vy_ += (fy_ - vy_) * beta;
    
    fx_ = 0.0;
    fy_ = 0.0;

    mapx_ = wrap(mapx_ + vx_ * dt, 0.0f, 1.0f);
    mapy_ = clamp(mapy_ + vy_ * dt, 0.0f, 1.0f);

    level_ += (target_level_ - level_) * beta;
    if (std::abs(level_ - target_level_) < 1.0f/256)
    {
        level_ = target_level_;
    }
    level_ = clamp(level_, 0.0f, static_cast<float>(max_level_));
    scale_ = std::pow(2.0f, level_);

    current_fps_ = 1.0f / dt;
    smoothed_fps_ += (current_fps_ - smoothed_fps_) * beta;
}

void osmview::Mapview::render_hud()
{
    SDL_Color color{128, 128, 128, 0};
    int line = 0;
    int linespacing = font_.GetLineSkip() * 1.5f;
    {
        // coordinates
        std::ostringstream oss;
        point_latlon latlon = xy2latlon({mapx_, mapy_});
        oss << latlon.lat <<", " << latlon.lon;
        auto coord_texture = make_text_texture(oss.str(), color);
        renderer_.RenderTexture(coord_texture, SDL3::WholeSurface{}, SDL3::FPoint{0.0f, float((line++)*linespacing)});
    }
    {
        // cache size
        //TODO(ipopov): add interface to layer to get some stats
        //std::ostringstream oss;
        //oss << "Cache size: " << cache_->size();
        //auto coord_texture = make_text_texture(oss.str(), color);
        //renderer_.RenderTexture(coord_texture, std::nullopt, {0, (line++)*linespacing});
    }
    {
        // FPS
        std::ostringstream oss;
        oss << "FPS: " << std::setprecision(3) << smoothed_fps_;
        auto coord_texture = make_text_texture(oss.str(), color);
        renderer_.RenderTexture(coord_texture, SDL3::WholeSurface{}, SDL3::FPoint{0.0f, float((line++)*linespacing)});
    }
}

SDL3::Texture
osmview::Mapview::make_text_texture(const std::string &text,
                                    const SDL_Color &color)
{
    auto surface = font_.RenderText_Blended(text, color);
    return SDL3::Texture(renderer_, surface);
}
