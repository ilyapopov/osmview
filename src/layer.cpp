#include "layer.hpp"

osmview::Layer::Layer(std::string name) : name_(std::move(name)) {}

osmview::Layer::~Layer() = default;
