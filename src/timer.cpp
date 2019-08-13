#include "timer.hpp"

#include <ostream>
#include <string>
#include <utility>

osmview::Timer::~Timer() = default;

osmview::DeltaTimer::~DeltaTimer() = default;

osmview::ScopedTimer::ScopedTimer(std::string label, std::ostream &stream)
    : label_(std::move(label)), stream_(stream)
{
    stream_ << label_ << " started" << std::endl;
}

osmview::ScopedTimer::~ScopedTimer()
{
    stream_ << label_ << " finished in " << time() << " s" << std::endl;
}
