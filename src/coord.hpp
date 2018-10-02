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

#ifndef COORD_HPP_INCLUDED
#define COORD_HPP_INCLUDED

namespace osmview
{

struct point_xy
{
    double x;
    double y;
};

struct point_latlon
{
    double lat;
    double lon;
};

point_latlon xy2latlon(point_xy xy);
point_xy latlon2xy(point_latlon latlon);

template <typename T>
inline T clamp(const T &x, const T &a, const T &b)
{
    if (x < a) {
        return a;
    }
    if (b < x) {
        return b;
    }
    return x;
}

template <typename T>
inline T wrap(const T &x, const T &a, const T &b)
{
    T y = x;
    while (y < a)
    {
        y += b - a;
    }
    while (b < y || b == y)
    {
        y -= b - a;
    }
    return y;
}

} // namespace osmview

#endif
