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

#ifndef COORD_HPP_INCLUDED
#define COORD_HPP_INCLUDED

namespace osmview
{

double lon2mapx(double lon);
double lat2mapy(double lat);
double mapx2lon(double x);
double mapy2lat(double y);

template <typename T>
T clamp(const T &x, const T &a, const T &b)
{
    if (x < a) return a;
    if (b < x) return b;
    return x;
}

template <typename T>
T wrap(const T &x, const T &a, const T &b)
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

}

#endif
