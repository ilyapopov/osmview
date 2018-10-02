#ifndef HTTP_TILE_SOURCE_HPP
#define HTTP_TILE_SOURCE_HPP

#include "tile_source.hpp"

namespace osmview
{

class http_tile_source : public tile_source
{
public:
    http_tile_source();
    SDL2pp::Optional<shared_buffer> get_tile(TileId id) override;
    void prefetch(TileId id) override;
};

} // namespace osmview

#endif // HTTP_TILE_SOURCE_HPP
