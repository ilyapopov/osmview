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

#include "coord.hpp"

#include <cmath>

double osmview::lon2mapx(double lon)
{ 
	return (lon + 180.0) / 360.0;
}
 
double osmview::lat2mapy(double lat)
{
    double lat_rad = lat * M_PI/180.0;
    return (1.0 - std::asinh(std::tan(lat_rad)) / M_PI) / 2.0;
}
 
double osmview::mapx2lon(double x)
{
	return x * 360.0 - 180.0;
}
 
double osmview::mapy2lat(double y)
{
	double n = M_PI - 2.0 * M_PI * y;
    return 180.0 / M_PI * std::atan(std::sinh(n));
}
