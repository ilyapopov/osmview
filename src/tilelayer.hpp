#ifndef TILELAYER_HPP
#define TILELAYER_HPP

#include "filesystem.hpp"
#include "layer.hpp"
#include "tilecache.hpp"
#include "timer.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "sdl3.hpp"

namespace osmview { class TileId; }

namespace osmview
{

class TileLayer : public Layer
{
    TileCache cache_;
    std::size_t tile_size_;

    enum class VisitOrder
    {
        by_column,
        from_center,
    };

    std::vector<std::pair<TileId, SDL3::FRect>>
    visible_tiles(int tile_level, float level,
                  const point_xy &map_pos,
                  SDL3::Renderer &renderer, VisitOrder order = VisitOrder::from_center) const;

public:
    TileLayer(const std::string &name, const std::string &url,
              const fs::path &cache_dir, std::size_t tile_size,
              SDL3::Renderer &renderer);
    ~TileLayer() override;

    void render(float level, const point_xy &map_pos,
                SDL3::Renderer &renderer, const BudgetTimer &frame_timer) override;
};

} // namespace osmview

#endif // TILELAYER_HPP
