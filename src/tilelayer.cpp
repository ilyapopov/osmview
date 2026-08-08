#include "tilelayer.hpp"

#include "coord.hpp"           // for wrap
#include "tile_id.hpp"         // for TileId
#include "timer.hpp"

#include "sdl3.hpp"

#include <algorithm>           // for max, min, sort
#include <cmath>               // for round, ceil, floor, pow

osmview::TileLayer::TileLayer(const std::string &name,
                              const std::string &url,
                              const fs::path &cache_dir, size_t tile_size,
                              SDL3::Renderer &renderer)
    : Layer(name), cache_(cache_dir, url, renderer), tile_size_(tile_size)
{
}

osmview::TileLayer::~TileLayer() = default;

void osmview::TileLayer::render(float level,
                                const point_xy &map_pos,
                                SDL3::Renderer &renderer,
                                const BudgetTimer& frame_timer)
{
    int tile_level = std::round(level);

    for (const auto & p: visible_tiles(tile_level, level, map_pos, renderer, VisitOrder::from_center))
    {
        auto & texture = cache_.get_texture(p.first, frame_timer.still_have_time());
        renderer.RenderTexture(texture, SDL3::WholeSurface{}, p.second);
    }
}

namespace
{

inline int length_squared(const SDL3::FPoint & p)
{
    return p.x*p.x + p.y*p.y;
}

} // namespace

std::vector<std::pair<osmview::TileId, SDL3::FRect> >
osmview::TileLayer::visible_tiles(int tile_level, float level,
                                  const point_xy &map_pos,
                                  SDL3::Renderer &renderer,
                                  osmview::TileLayer::VisitOrder order) const
{
    SDL3::Point output_size = renderer.GetOutputSize();
    SDL3::FPoint center = SDL3::FPoint{static_cast<float>(output_size.x/2), static_cast<float>(output_size.y/2)};

    float w = output_size.x;
    float h = output_size.y;

    float tile_scale = std::pow(2.0f, tile_level);
    int n = 1 << tile_level;

    float tile_draw_scale = std::pow(2.0f, level - tile_level);
    float scaled_size = tile_draw_scale * tile_size_;

    float xc = map_pos.x * tile_scale;
    float yc = map_pos.y * tile_scale;

    float xmin = xc - 0.5f * w / scaled_size;
    float xmax = xc + 0.5f * w / scaled_size;
    float ymin = yc - 0.5f * h / scaled_size;
    float ymax = yc + 0.5f * h / scaled_size;

    int imin = std::floor(xmin);
    int imax = std::ceil(xmax);
    int jmin = std::max(static_cast<int>(std::floor(ymin)), 0);
    int jmax = std::min(static_cast<int>(std::ceil(ymax)), n);

    std::vector<std::pair<TileId, SDL3::FRect>> tiles;
    tiles.reserve((output_size.x / scaled_size + 1)
                             * (output_size.y / scaled_size + 1));

    for(int i = imin; i < imax; ++i)
    {
        int i1 = wrap(i, 0, n);

        for(int j = jmin; j < jmax; ++j)
        {
            float a = (0.5f * w + scaled_size * (i - xc));
            float b = (0.5f * h + scaled_size * (j - yc));
            SDL3::FRect rect{a, b, scaled_size, scaled_size};

            tiles.emplace_back(TileId(tile_level, i1, j), rect);
        }
    }

    if (order == VisitOrder::from_center)
    {
        std::sort(tiles.begin(), tiles.end(),
                  [&](const std::pair<TileId, SDL3::FRect> & lhs,
                  const std::pair<TileId, SDL3::FRect> & rhs){
            return length_squared(lhs.second.GetCentroid() - center)
                    < length_squared(rhs.second.GetCentroid() - center);
        });
    }

    return tiles;
}
