#ifndef LAYER_HPP
#define LAYER_HPP

#include "coord.hpp"

#include <array>
#include <string>
#include <utility>

namespace SDL2pp
{
class Renderer;
} // namespace SDL2pp

namespace osmview
{

class Layer
{
    std::string name_;

public:
    explicit Layer(std::string name);
    virtual ~Layer();

    const std::string &name() const { return name_; }
    virtual void render(double level, const point_xy &map_pos,
                        SDL2pp::Renderer &renderer) = 0;
};

} // namespace osmview

#endif // LAYER_HPP
