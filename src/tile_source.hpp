#ifndef TILE_SOURCE_HPP
#define TILE_SOURCE_HPP

#include <memory>
#include "SDL2pp/Optional.hh"

#include "tile_id.hpp"

namespace osmview {

class shared_buffer {
    std::shared_ptr<char[]> data_;
    size_t size_;

public:
    shared_buffer(std::shared_ptr<char[]> data, size_t size)
        : data_(std::move(data)), size_(size)
    {}

    char * data() const
    {
        return data_.get();
    }

    size_t size() const
    {
        return size_;
    }
};

class tile_source
{
public:
    virtual ~tile_source() = 0;
    virtual SDL2pp::Optional<shared_buffer> get_tile(TileId id) = 0;
    virtual void prefetch(TileId id) = 0;
};

} // namespace osmview

#endif // TILE_SOURCE_HPP
