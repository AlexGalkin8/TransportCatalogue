#pragma once

namespace geo
{
    struct Coordinates
    {
        double lat = 0.0;
        double lng = 0.0;
    };

    inline bool operator==(const Coordinates& lhs, const Coordinates& rhs);

    double ComputeDistance(Coordinates from, Coordinates to);
}