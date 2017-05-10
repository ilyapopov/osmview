/*
    Copyright 2011, 2014, 2015, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.
    https://bitbucket.org/ipopov/osmview

    osmview is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    osmview is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with osmview.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

#include <chrono>
#include <string>
#include <ostream>
#include <utility>

namespace osmview
{

class Timer
{
protected:
    using clock_type = std::chrono::steady_clock;
    using time_point_type = std::chrono::time_point<clock_type>;

    time_point_type started_;

    static time_point_type now()
    {
        return clock_type::now();
    }

public:

    Timer(): started_(now())
    {}

    virtual ~Timer()
    {}
    
    double time() const
    {
        return std::chrono::duration_cast<std::chrono::duration<double> >(
                    now() - started_
                    ).count();
    }
};

class DeltaTimer : public Timer
{
    time_point_type last_;

public:
    DeltaTimer() : last_(started_)
    {}

    ~DeltaTimer() {}

    double delta()
    {
        auto tmp = std::exchange(last_, now());
        return std::chrono::duration_cast<std::chrono::duration<double> >(
                last_ - tmp).count();
    }
};

class ScopedTimer : public Timer
{
    std::string label_;
    std::ostream & stream_;

public:
    explicit ScopedTimer(std::string label, std::ostream & stream)
        : label_(std::move(label)), stream_(stream)
    {
        stream_ << label_ << " started" << std::endl;
    }

    ~ScopedTimer()
    {
        stream_ << label_ << " finished in " << time() << " s" << std::endl;
    }
};

} // namespace

#endif
