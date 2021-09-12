#include "tilelayer.hpp"

#include "coord.hpp"           // for wrap
#include "tile_id.hpp"         // for TileId

#include <SDL2pp/Optional.hh>  // for NullOpt
#include <SDL2pp/Point.hh>     // for Point
#include <SDL2pp/Rect.hh>      // for Rect
#include <SDL2pp/Renderer.hh>  // for Renderer

#include <algorithm>           // for max, min, sort
#include <cmath>               // for round, ceil, floor, pow

osmview::TileLayer::TileLayer(const std::string &name,
                              const std::string &url,
                              const fs::path &cache_dir, size_t tile_size,
                              SDL2pp::Renderer &renderer)
    : Layer(name), cache_(cache_dir, url, renderer), tile_size_(tile_size)
{
}

osmview::TileLayer::~TileLayer() = default;

void osmview::TileLayer::render(double level,
                                const point_xy &map_pos,
                                SDL2pp::Renderer &renderer)
{
    int tile_level = std::round(level);

    for (const auto & p: visible_tiles(tile_level, level, map_pos, renderer, VisitOrder::from_center))
    {
        auto & texture = cache_.get_texture(p.first);
        renderer.Copy(texture, SDL2pp::NullOpt, p.second);
    }
}

namespace
{

inline int length_squared(const SDL2pp::Point & p)
{
    return p.x*p.x + p.y*p.y;
}

} // namespace

std::vector<std::pair<osmview::TileId, SDL2pp::Rect> >
osmview::TileLayer::visible_tiles(int tile_level, double level,
                                  const point_xy &map_pos,
                                  SDL2pp::Renderer &renderer,
                                  osmview::TileLayer::VisitOrder order) const
{
    auto output_size = renderer.GetOutputSize();

    double w = output_size.x;
    double h = output_size.y;

    double tile_scale = std::pow(2.0, tile_level);
    int n = 1 << tile_level;

    double tile_draw_scale = std::pow(2.0, level - tile_level);
    int scaled_size = tile_draw_scale * tile_size_;

    double xc = map_pos.x * tile_scale;
    double yc = map_pos.y * tile_scale;

    double xmin = xc - 0.5 * w / scaled_size;
    double xmax = xc + 0.5 * w / scaled_size;
    double ymin = yc - 0.5 * h / scaled_size;
    double ymax = yc + 0.5 * h / scaled_size;

    int imin = std::floor(xmin);
    int imax = std::ceil(xmax);
    int jmin = std::max(static_cast<int>(std::floor(ymin)), 0);
    int jmax = std::min(static_cast<int>(std::ceil(ymax)), n);

    std::vector<std::pair<TileId, SDL2pp::Rect>> tiles;
    tiles.reserve((output_size.x / scaled_size + 1)
                             * (output_size.y / scaled_size + 1));

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
            return length_squared(lhs.second.GetCentroid() - output_size/2)
                    < length_squared(rhs.second.GetCentroid() - output_size/2);
        });
    }

    return tiles;
}
