#include "timer.hpp"

#include <time.h>

Timer::Timer()
: _started(now()), _last(_started)
{
}

double Timer::now() const
{
    timespec ts;
    
    clock_gettime(CLOCK_MONOTONIC, &ts);
    
    return ts.tv_sec + 1e-09 * ts.tv_nsec;
}


