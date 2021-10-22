#include "domain.h"

using namespace geo;
using namespace std;


namespace transport_catalogue::objects
{
    bool operator==(const Bus& lhs, const Bus& rhs)
    {
        return lhs.number == rhs.number;
    }


    bool operator<(const Bus& lhs, const Bus& rhs)
    {
        return lhs.number < rhs.number;
    }


    bool operator==(const Stop& lhs, const Stop& rhs)
    {
        return lhs.stop_name == rhs.stop_name;
    }


    bool operator<(const Road& lhs, const Road& rhs)
    {
        return lhs.minutes < rhs.minutes;
    }


    bool operator>(const Road& lhs, const Road& rhs)
    {
        return lhs.minutes > rhs.minutes;
    }


    Road operator+(const Road& lhs, const Road& rhs)
    {
        return { {}, lhs.minutes + rhs.minutes, {} };
    }


    size_t HashPairStops::operator() (const pair<shared_ptr<Stop>, shared_ptr<Stop>>& pair) const
    {
        size_t p_hash_first = p_hasher(pair.first.get());
        size_t p_hash_second = p_hasher(pair.second.get());
        return p_hash_first + p_hash_second * 10;
    }

} // namespace transport_catalogue::objects
