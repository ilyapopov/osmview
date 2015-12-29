#ifndef OSMVIEW_TILEID_HPP
#define OSMVIEW_TILEID_HPP

#include <cstdint>
#include <functional>
#include <ostream>

namespace osmview
{

class TileId
{
    constexpr static uint64_t coord_mask_ = 0x3fffff;

    uint64_t id_;

public:
    constexpr TileId(int level, int x, int y) noexcept:
        id_(((uint64_t)level << 44) | ((uint64_t)x << 22) | (uint64_t)y)
    {}

    constexpr uint64_t id() const {return id_;}
    constexpr int level() const {return (int)(id_ >> 44);}
    constexpr int x() const {return (int)((id_ >> 22) & coord_mask_);}
    constexpr int y() const {return (int)(id_ & coord_mask_);}

    constexpr bool operator==(TileId other)
    {
        return id_ == other.id_;
    }
};

inline std::ostream & operator<<(std::ostream & os, TileId id)
{
    return os << id.level() << '/' << id.x() << '/' << id.y();
}

inline constexpr bool operator<(TileId lhs, TileId rhs)
{
    return lhs.id() < rhs.id();
}

} // namespace

/////////////////////////////////////////////////////////////////////////////

namespace std
{

template<>
struct hash<osmview::TileId>
{
    size_t operator()(osmview::TileId id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace


#endif // OSMVIEW_TILEID_HPP

