#include "geo.h"

#define _USE_MATH_DEFINES
#include <cmath>


namespace geo
{
    double ComputeDistance(Coordinates from, Coordinates to)
    {
        using namespace std;
        static const size_t EARTH_RADIUS = 6371000;
        static const double DEGREE = M_PI / 180.0;
        return acos(sin(from.lat * DEGREE) * sin(to.lat * DEGREE)
            + cos(from.lat * DEGREE) * cos(to.lat * DEGREE)
            * cos(abs(from.lng - to.lng) * DEGREE)) * EARTH_RADIUS;
    }


    inline bool operator==(const Coordinates& lhs, const Coordinates& rhs)
    {
        return lhs.lat == rhs.lat && lhs.lng == rhs.lng;
    }

}  // namespace geo