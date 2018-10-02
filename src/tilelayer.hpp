#ifndef TILELAYER_HPP
#define TILELAYER_HPP

#include "filesystem.hpp"
#include "layer.hpp"
#include "tilecache.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "SDL2pp/Rect.hh"

namespace SDL2pp { class Rect; }
namespace SDL2pp { class Renderer; }
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

    std::vector<std::pair<TileId, SDL2pp::Rect>>
    visible_tiles(int tile_level, double level,
                  const point_xy &map_pos,
                  SDL2pp::Renderer &renderer, VisitOrder order = VisitOrder::from_center);

public:
    TileLayer(const std::string &name, const std::string &url,
              const fs::path &cache_dir, std::size_t tile_size,
              SDL2pp::Renderer &renderer);
    ~TileLayer() override;

    void render(double level, const point_xy &map_pos,
                SDL2pp::Renderer &renderer) override;
};

} // namespace osmview

#endif // TILELAYER_HPP
