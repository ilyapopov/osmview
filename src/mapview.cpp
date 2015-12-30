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

#include <algorithm>
#include <cmath>
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
    font_("data/DejaVuSans.ttf", 12)
{
    std::string server_name("tile.openstreetmap.org");

    cache_.reset(new TileCache(get_user_cache_dir() + "/maps/" + server_name,
                           "http://" + server_name,
                           renderer));

    SDL2pp::Surface credits_surface = font_.RenderUTF8_Shaded(
        "Cartography © OpenStreetMap contributors | CC BY-SA | openstreetmap.org",
        {64, 64, 64, 0}, {255, 255, 255, 0});
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

    for (const auto & p: visible_tiles(target_level_, VisitOrder::from_center))
    {
        cache_->prefetch(p.first);
    }

    return target_level_;
}

void osmview::Mapview::render()
{
    int tile_level = std::round(level_);

    for (const auto & p: visible_tiles(tile_level, VisitOrder::from_center))
    {
        auto & texture = cache_->get_texture(p.first);
        renderer_.Copy(texture, SDL2pp::NullOpt, p.second);
    }

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

namespace
{
    inline int length_squared(const SDL2pp::Point & p)
    {
        return p.x*p.x + p.y*p.y;
    }
}

std::vector<std::pair<osmview::TileId, SDL2pp::Rect> >
osmview::Mapview::visible_tiles(int tile_level,
                                osmview::Mapview::VisitOrder order)
{
    output_size_ = renderer_.GetOutputSize();

    double w = output_size_.x;
    double h = output_size_.y;

    double tile_scale = std::pow(2.0, (double)tile_level);
    int n = 1 << tile_level;

    double tile_draw_scale = std::pow(2.0, level_ - tile_level);
    int scaled_size = tile_draw_scale * tile_size_;

    double xc = mapx_ * tile_scale;
    double yc = mapy_ * tile_scale;

    double xmin = xc - 0.5 * w / scaled_size;
    double xmax = xc + 0.5 * w / scaled_size;
    double ymin = yc - 0.5 * h / scaled_size;
    double ymax = yc + 0.5 * h / scaled_size;

    int imin = std::floor(xmin);
    int imax = std::ceil(xmax);
    int jmin = std::max((int)std::floor(ymin), 0);
    int jmax = std::min((int)std::ceil(ymax), n);

    std::vector<std::pair<TileId, SDL2pp::Rect>> tiles;
    tiles.reserve((output_size_.x / scaled_size + 1)
                             * (output_size_.y / scaled_size + 1));

    for(int i = imin; i < imax; ++i)
    {
        int i1 = wrap(i, 0, n);

        for(int j = jmin; j < jmax; ++j)
        {
            int a = std::round((w/2) + scaled_size * (i - xc));
            int b = std::round((h/2) + scaled_size * (j - yc));
            SDL2pp::Rect rect(a, b, scaled_size, scaled_size);

            tiles.emplace_back(TileId(tile_level, i1, j), rect);
        }
    }

    if (order == VisitOrder::from_center)
    {
        std::sort(tiles.begin(), tiles.end(),
                  [&](const std::pair<TileId, SDL2pp::Rect> & lhs,
                  const std::pair<TileId, SDL2pp::Rect> & rhs){
            return length_squared(lhs.second.GetCentroid() - output_size_/2)
                    < length_squared(rhs.second.GetCentroid() - output_size_/2);
        });
    }

    return tiles;
}
