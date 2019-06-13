#include "layer.hpp"

#include <utility>

osmview::Layer::Layer(std::string name) : name_(std::move(name)) {}

osmview::Layer::~Layer() = default;
