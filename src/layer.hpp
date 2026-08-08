#ifndef LAYER_HPP
#define LAYER_HPP

#include "coord.hpp"
#include "timer.hpp"

#include <string>
#include "sdl3.hpp"


namespace osmview
{

class Layer
{
    std::string name_;

public:
    explicit Layer(std::string name);
    virtual ~Layer();

    const std::string &name() const { return name_; }
    virtual void render(float level, const point_xy &map_pos,
                        SDL3::Renderer &renderer, const BudgetTimer &frame_timer) = 0;
};

} // namespace osmview

#endif // LAYER_HPP
