#include "domain.h"

#include <utility>
#include <functional>
#include <algorithm>


using namespace geo;

namespace transport_catalogue
{
    namespace objects
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


        size_t HashPairStops::operator() (const std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>& pair) const
        {
            size_t p_hash_first = p_hasher(pair.first.get());
            size_t p_hash_second = p_hasher(pair.second.get());
            return p_hash_first + p_hash_second * 10;
        }


    } // namespace objects




    namespace database
    {
        using namespace objects;

        const std::shared_ptr<Bus> DataBase::FindBus(const std::string& bus_name) const
        {
            return buses_.count(bus_name) ? buses_.at(bus_name) : nullptr;
        }


        const std::shared_ptr<Stop> DataBase::FindStop(const std::string& name) const
        {
            return stops_.count(name) ? stops_.at(name) : nullptr;
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

        void DataBase::SetDistanceBetweenStops(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to, size_t distance)
        {
            length_between_stops_[std::make_pair(from, to)] = distance;
        }

        size_t DataBase::GetDistanceBetweenStops(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to) const
        {
            auto pair = std::make_pair(from, to);
            if (length_between_stops_.count(pair) != 0)
                return length_between_stops_.at(pair);

            pair = std::make_pair(to, from);
            if (length_between_stops_.count(pair) != 0)
                return length_between_stops_.at(pair);

            return 0;
        }


    } // namespace database

} // namespace transport_catalogue
