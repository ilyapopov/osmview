#include "coord.hpp"

#include <cmath>

double lon2tilex(double lon, int z) 
{ 
	return (lon + 180.0) / 360.0 * pow(2.0, z);
}
 
double lat2tiley(double lat, int z)
{ 
	return (1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z);
}
 
double tilex2lon(double x, int z) 
{
	return x / pow(2.0, z) * 360.0 - 180.0;
}
 
double tiley2lat(double y, int z) 
{
	double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}
