#include "domain.h"

#include <utility>
#include <functional>
#include <algorithm>

using namespace geo;

namespace transport_catalogue
{
    namespace objects
    {
        inline bool operator==(const Bus& lhs, const Bus& rhs)
        {
            return lhs.number == rhs.number;
        }


        inline bool operator<(const Bus& lhs, const Bus& rhs)
        {
            return lhs.number < rhs.number;
        }


        inline bool operator==(const Stop& lhs, const Stop& rhs)
        {
            return lhs.stop_name == rhs.stop_name;
        }
    } // namespace objects



    namespace database
    {
        using namespace objects;

        const std::shared_ptr<Bus> DataBase::FindBus(const std::string& bus_name) const
        {
            return buses_.count(bus_name) ? buses_.at(bus_name) : nullptr;
        }


        const std::shared_ptr<Stop> DataBase::FindStop(const std::string& stop_name) const
        {
            return stops_.count(stop_name) ? stops_.at(stop_name) : nullptr;
        }


        const Buses& DataBase::GetBuses() const
        {
            return buses_;
        }


        const Stops& DataBase::GetStops() const
        {
            return stops_;
        }


        void DataBase::AddStop(const Stop& stop)
        {
            if (stops_.count(stop.stop_name))
                stops_.at(stop.stop_name)->coordinates = stop.coordinates;
            else
                stops_.emplace(stop.stop_name, std::make_shared<Stop>(stop));
        }


        void DataBase::AddBus(const Bus& bus)
        {
            Route route;

            // заполняем остановки маршрута
            for (std::shared_ptr<Stop> stop : bus.bus_route.route_stops)
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


        void DataBase::AddDistanceBetweenStops(std::string& stop_name, std::map<std::string, size_t>& distances_for_stop)
        {
            length_between_stops_[stop_name] = std::make_shared<std::map<std::string, size_t>>(std::move(distances_for_stop));
        }


        const std::shared_ptr<std::map<std::string, size_t>>  DataBase::GetDistanceBetweenStops(std::string& stop_name) const
        {
            return length_between_stops_.count(stop_name) ? length_between_stops_.at(stop_name) : nullptr;
        }
    } // namespace database

} // namespace transport_catalogue