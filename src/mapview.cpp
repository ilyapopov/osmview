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

#include "mapview.hpp"

#include <cmath>
#include <string>

#include <SDL2pp/Renderer.hh>

#include "coord.hpp"
#include "tilecache.hpp"

const std::string osmview::Mapview::tile_dir_("/home/ipopov/.cache/maps/tile.openstreetmap.org/");
const std::string osmview::Mapview::url_base_("http://tile.openstreetmap.org/");
const double osmview::Mapview::v0_ = 2.0;
const double osmview::Mapview::tau_ = 0.3;

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
    cache_(new TileCache(tile_dir_, url_base_, renderer)),
    renderer_(renderer), output_size_(renderer_.GetOutputSize()),
    frame_num_(0)
{
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
    double scale = std::pow(2.0, target_level_);
    fx_ += v0_ * move_x / scale;
    fy_ += v0_ * move_y / scale;
}

void osmview::Mapview::move_pix_hard(double dx, double dy)
{
    double scale = std::pow(2.0, target_level_);
    mapx_ += dx / tile_size_ / scale;
    mapy_ += dy / tile_size_ / scale;
}

int osmview::Mapview::zoom(int step)
{
    target_level_ = clamp(target_level_ + step, 0, max_level_);
        
    return target_level_;
}

void osmview::Mapview::render()
{
    ++frame_num_;

    output_size_ = renderer_.GetOutputSize();

    int w = output_size_.x;
    int h = output_size_.y;

    int tile_level = std::round(level_);
    double tile_scale = std::pow(2.0, (double)tile_level);
    int n = 1 << tile_level;

    double tile_draw_scale = std::pow(2.0, level_ - tile_level);
    int scaled_size = tile_draw_scale * tile_size_;

    double xc = mapx_ * tile_scale;
    double yc = mapy_ * tile_scale;
    
    double xmin = xc - 0.5 * w / tile_size_;
    double xmax = xc + 0.5 * w / tile_size_;
    double ymin = yc - 0.5 * h / tile_size_;
    double ymax = yc + 0.5 * h / tile_size_;
    
    int imin = std::floor(xmin);
    int imax = std::ceil(xmax);
    int jmin = std::max((int)std::floor(ymin), 0);
    int jmax = std::min((int)std::ceil(ymax), n);

    for(int i = imin; i < imax; ++i)
    {
        int i1 = wrap(i, 0, n);

        for(int j = jmin; j < jmax; ++j)
        {
            auto & tile = cache_->get_texture(tile_level, i1, j, frame_num_);

            int a = std::floor((w/2) + scaled_size * (i - xc));
            int b = std::floor((h/2) + scaled_size * (j - yc));
                
            SDL2pp::Rect rect = {a, b, scaled_size, scaled_size};
            renderer_.Copy(tile, SDL2pp::NullOpt, rect);
        }
    }
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
    level_ = clamp(level_, 0.0, (double)max_level_);
    scale_ = std::pow(2.0, level_);
}
