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
#include <iosfwd>
#include <string>
#include <utility>

namespace osmview
{

class Timer
{
public:
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;
    using duration = clock_type::duration;

protected:
    time_point started_;

public:
    Timer(): started_(clock_type::now())
    {}

    virtual ~Timer();
    
    double time() const
    {
        return std::chrono::duration_cast<std::chrono::duration<double> >(
                   clock_type::now() - started_
                    ).count();
    }
};

class DeltaTimer : public Timer
{
    time_point last_;

public:
    DeltaTimer() : last_(started_)
    {}

    ~DeltaTimer() override;

    double delta()
    {
        auto tmp = std::exchange(last_, clock_type::now());
        return std::chrono::duration_cast<std::chrono::duration<double> >(
                last_ - tmp).count();
    }
};

class ScopedTimer : public Timer
{
    std::string label_;
    std::ostream & stream_;

public:
    explicit ScopedTimer(std::string label, std::ostream & stream);

    ~ScopedTimer() override;
};

class BudgetTimer{
public:
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;
    using duration = clock_type::duration;

private:
    time_point deadline_;

public:
    explicit BudgetTimer(time_point deadline)
        : deadline_(deadline)
    {}
    explicit BudgetTimer(duration budget)
        : deadline_(clock_type::now() + budget)
    {}

    bool still_have_time() const {
        return clock_type::now() < deadline_;
    }

    duration time_left() const {
        auto now_ = clock_type::now();
        if (now_ >= deadline_) {
            return duration::zero();
        }
        return deadline_ - now_;
    }
};

} // namespace osmview

#endif
