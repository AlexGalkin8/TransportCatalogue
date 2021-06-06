#pragma once

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

using namespace std::string_literals;

struct Bus;
struct Stop;
class TransportCatalogue;


struct Stop
{
    std::string stop_name_;
    std::deque<Bus> buses_;
    std::pair <double, double> coordinate; // latitude and longitude.
};


struct Bus
{
    uint32_t bus_number_;
    std::deque<Stop> route_;
};


class TransportCatalogue
{
public:
    void AddStop(Stop stop);
    void AddBus(Bus bus);

    std::string GetBusInfo(std::string bus_number);
    

private:
    std::unordered_map <Bus, std::deque<Stop> route_>;
};


