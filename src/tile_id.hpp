#ifndef OSMVIEW_TILE_ID_HPP
#define OSMVIEW_TILE_ID_HPP

#include <cstddef>
#include <cstdint>
#include <functional>

namespace osmview
{

class TileId
{
    constexpr static uint64_t coord_mask_ = 0x3fffff;

    uint64_t id_;

public:
    constexpr TileId(int level, int x, int y) noexcept:
        id_((static_cast<uint64_t>(level) << 44) | (static_cast<uint64_t>(x) << 22) | static_cast<uint64_t>(y))
    {}

    constexpr uint64_t id() const {return id_;}
    constexpr int level() const {return static_cast<int>(id_ >> 44);}
    constexpr int x() const {return static_cast<int>((id_ >> 22) & coord_mask_);}
    constexpr int y() const {return static_cast<int>(id_ & coord_mask_);}

    friend constexpr bool operator==(TileId lhs, TileId rhs)
    {
        return lhs.id_ == rhs.id_;
    }

    friend constexpr bool operator!=(TileId lhs, TileId rhs)
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(TileId lhs, TileId rhs)
    {
        return lhs.id_ < rhs.id_;
    }
};

} // namespace osmview

/////////////////////////////////////////////////////////////////////////////

namespace std
{

template<>
struct hash<osmview::TileId>
{
    using argument_type = osmview::TileId;
    using result_type = std::size_t;
    size_t operator()(osmview::TileId id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std


#endif // OSMVIEW_TILE_ID_HPP

