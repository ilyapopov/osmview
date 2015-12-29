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

#include <cstdlib>

#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>

#include "tilecache.hpp"

const double osmview::Mapview::v0_ = 2.0;
const double osmview::Mapview::tau_ = 0.3;
const int osmview::Mapview::tile_size_;
const int osmview::Mapview::max_level_;

namespace
{

std::string get_user_cache_dir()
{
    const char * d = nullptr;
    // Using "XDG Base Directory Specification"
    if ((d = std::getenv("XDG_CACHE_HOME")))
        return d;
    // Use default directory
    if ((d = std::getenv("HOME")))
        return std::string(d) + "/.cache";
    throw std::runtime_error("Cannot figure out cache directory location");
}

} // namespace

SDL2pp::Point osmview::Mapview::to_screen(double x, double y)
{
    int x1 = std::round((x - mapx_) * scale_);
    int y1 = std::round((y - mapy_) * scale_);
    return SDL2pp::Point(x1, y1) + output_size_ / 2;
}

std::pair<double, double> osmview::Mapview::from_screen(const SDL2pp::Point &p)
{
    SDL2pp::Point p1 = p - output_size_ / 2;
    return {mapx_ + p1.x / scale_, mapy_ + p1.y / scale_};
}

osmview::Mapview::Mapview(SDL2pp::Renderer &renderer)
    : mapx_(0.5), mapy_(0.5),
    vx_(0.0), vy_(0.0), fx_(0.0), fy_(0.0),
    target_level_(1), level_(1.0), scale_(1.0),
    renderer_(renderer), output_size_(renderer_.GetOutputSize()),
    frame_num_(0),
    font_("data/ClearSans-Medium.ttf", 12)
{
    std::string server_name("tile.openstreetmap.org");

    cache_.reset(new TileCache(get_user_cache_dir() + "/maps/" + server_name,
                           "http://" + server_name,
                           renderer));

    SDL2pp::Surface credits_surface = font_.RenderUTF8_Shaded(
        "Cartography © OpenStreetMap contributors | CC BY-SA | openstreetmap.org",
        {128, 128, 128, 0}, {255, 255, 255, 0});
    credits_texture_.emplace(renderer_, credits_surface);
}

osmview::Mapview::~Mapview()
{}

void osmview::Mapview::center_on_latlon(double lat, double lon)
{
    mapx_ = lon2mapx(lon);
    mapy_ = lat2mapy(lat);
    vx_ = vy_ = 0.0;
    fx_ = fy_ = 0.0;
}

void osmview::Mapview::move(double move_x, double move_y)
{
    fx_ += v0_ * move_x / scale_;
    fy_ += v0_ * move_y / scale_;
}

void osmview::Mapview::move_pix_hard(double dx, double dy)
{
    mapx_ += dx / tile_size_ / scale_;
    mapy_ += dy / tile_size_ / scale_;
}

int osmview::Mapview::zoom(int step)
{
    target_level_ = clamp(target_level_ + step, 0, max_level_);

    for_all_tiles(target_level_,
                  [&](TileId tile_id, const SDL2pp::Rect & /*rect*/)
    {
        cache_->prefetch(tile_id, frame_num_);
    });


    return target_level_;
}

void osmview::Mapview::render()
{
    ++frame_num_;
    int tile_level = std::round(level_);

    for_all_tiles(tile_level,
                  [&](TileId tile_id, const SDL2pp::Rect & rect)
    {
        auto & texture = cache_->get_texture(tile_id, frame_num_);
        renderer_.Copy(texture, SDL2pp::NullOpt, rect);
    });

    SDL2pp::Rect rect(output_size_ - credits_texture_->GetSize(),
                      credits_texture_->GetSize());
    renderer_.Copy(*credits_texture_, SDL2pp::NullOpt, rect);
}

void osmview::Mapview::update(double dt)
{
    double beta = 1.0 - std::exp(-dt / tau_);

    vx_ += (fx_ - vx_) * beta;
    vy_ += (fy_ - vy_) * beta;
    
    fx_ = 0.0;
    fy_ = 0.0;

    mapx_ = wrap(mapx_ + vx_ * dt, 0.0, 1.0);
    mapy_ = clamp(mapy_ + vy_ * dt, 0.0, 1.0);

    level_ += (target_level_ - level_) * beta;
    if (std::abs(level_ - target_level_) < 0.01)
        level_ = target_level_;
    level_ = clamp(level_, 0.0, (double)max_level_);
    scale_ = std::pow(2.0, level_);
}


