#include "timer.hpp"

osmview::Timer::~Timer()
{}

osmview::DeltaTimer::~DeltaTimer() {}

osmview::ScopedTimer::ScopedTimer(std::string label, std::ostream &stream)
    : label_(std::move(label)), stream_(stream)
{
    stream_ << label_ << " started" << std::endl;
}

osmview::ScopedTimer::~ScopedTimer()
{
    stream_ << label_ << " finished in " << time() << " s" << std::endl;
}
