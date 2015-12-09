/*
    Copyright 2011, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.

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

namespace osmview
{

class Timer
{
    using clock_type = std::chrono::steady_clock;
    using time_point_type = std::chrono::time_point<clock_type>;

    time_point_type started_;
    time_point_type last_;

    static time_point_type now()
    {
        return clock_type::now();
    }

public:

    Timer()
        : started_(now()), last_(started_)
    {}
    
    double time() const
    {
        return std::chrono::duration_cast<std::chrono::duration<double> >(
                    now() - started_
                    ).count();
    }
    double delta()
    {
        auto tmp = last_;
        last_ = now();
        return std::chrono::duration_cast<std::chrono::duration<double> >(
                last_ - tmp).count();
    }
};

} // namespace

#endif
