#include "coord.hpp"

#include <cmath>

double lon2mapx(double lon) 
{ 
	return (lon + 180.0) / 360.0;
}
 
double lat2mapy(double lat)
{ 
	return (1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0;
}
 
double mapx2lon(double x)
{
	return x * 360.0 - 180.0;
}
 
double mapy2lat(double y) 
{
	double n = M_PI - 2.0 * M_PI * y;
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}
