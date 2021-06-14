#pragma once

#include <cmath>
#include <functional>

namespace transport_catalogue
{
    namespace coordinate_tools
    {
        struct Coordinates
        {
            double lat = 0.0;
            double lng = 0.0;

            Coordinates() = default;
            Coordinates(const Coordinates& other)
                : lat(other.lat), lng(other.lng)
            {
            }

            Coordinates& operator=(const Coordinates& rhs)
            {
                if (this == &rhs)
                    return *this;

                lat = rhs.lat;
                lng = rhs.lng;

                return *this;
            }
        };

        struct CoordinatesHasher
        {
            size_t operator()(const Coordinates& coordinates) const
            {
                return std::hash<double>{}(coordinates.lat) +
                    std::hash<double>{}(coordinates.lng);
            }
        };

        inline double ComputeDistance(Coordinates from, Coordinates to)
        {
            using namespace std;
            static const double dr = 3.1415926535 / 180.;
            return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
                * 6371000;
        }
    }
}