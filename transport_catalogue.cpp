#include "transport_catalogue.h"

#include <stdexcept>
#include <iterator>
#include <utility>
#include <functional>

using namespace geo;
using namespace transport_catalogue::objects;
using namespace std;


namespace transport_catalogue::database
{
    const std::shared_ptr<Bus> TransportCatalogue::FindBus(const string& bus_name) const
    {
        return buses_.count(bus_name) ? buses_.at(bus_name) : nullptr;
    }


    const std::shared_ptr<Stop> TransportCatalogue::FindStop(const string& name) const
    {
        return stops_.count(name) ? stops_.at(name) : nullptr;
    }


    const Buses& TransportCatalogue::GetBuses() const
    {
        return buses_;
    }


    const Stops& TransportCatalogue::GetStops() const
    {
        return stops_;
    }


    void TransportCatalogue::AddStop(const Stop& stop)
    {
        if (stops_.count(stop.stop_name))
            stops_.at(stop.stop_name)->coordinates = stop.coordinates;
        else
            stops_.emplace(stop.stop_name, std::make_shared<Stop>(stop));
    }


    void TransportCatalogue::AddBus(const Bus& bus)
    {
        Route route;

        // заполняем остановки маршрута
        for (shared_ptr<Stop> stop : bus.bus_route.route_stops)
        {
            if (stops_.count(stop->stop_name))
            {
                // если находим то добавляем указатель на остановку в маршрут
                stops_.at(stop->stop_name)->free = false;
                route.route_stops.push_back(stops_.at(stop->stop_name));
            }
            else
            {
                // если не находим, то добавляем остановку в базу, а затем указатель на остановку в маршрут
                stop->free = false;
                stops_.emplace(stop->stop_name, stop);
                route.route_stops.push_back(stops_.at(stop->stop_name));
            }
        }

        // круговой маршрут или нет
        route.is_roundtrip = bus.bus_route.is_roundtrip;

        // добавляем маршрут в каталог
        Bus tmp_bus{ bus.number, route };
        buses_[tmp_bus.number] = std::make_shared<Bus>(tmp_bus);
    }


    void TransportCatalogue::SetDistanceBetweenStops(shared_ptr<Stop> from, shared_ptr<Stop> to, size_t distance)
    {
        length_between_stops_[std::make_pair(from, to)] = distance;
    }


    size_t TransportCatalogue::GetDistanceBetweenStops(shared_ptr<Stop> from, shared_ptr<Stop> to) const
    {
        auto pair = std::make_pair(from, to);
        if (length_between_stops_.count(pair) != 0)
            return length_between_stops_.at(pair);

        pair = std::make_pair(to, from);
        if (length_between_stops_.count(pair) != 0)
            return length_between_stops_.at(pair);

        return 0;
    }


    const StopsDistance& TransportCatalogue::GetStopsDistance() const
    {
        return length_between_stops_;
    }

} // namespace transport_catalogue::database