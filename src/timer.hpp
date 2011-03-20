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

class Timer
{

    double _started;
    double _last;
    
public:

    Timer();
    
    double now() const;

    double time() const
    {
        return now() - _started;
    }
    double delta()
    {
        double lastlast = _last;
        _last = now();
        return _last - lastlast;
    }
};

#endif
