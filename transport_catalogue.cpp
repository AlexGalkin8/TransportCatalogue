#include "transport_catalogue.h"

#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <utility>
#include <functional>

using namespace transport_catalogue::coordinate_tools;

namespace transport_catalogue
{
    /************************************************
    ********************   Bus   ********************
    *************************************************/

    inline bool operator==(const Bus& lhs, const Bus& rhs)
    {
        return lhs.number == rhs.number;
    }


    inline bool operator<(const Bus& lhs, const Bus& rhs)
    {
        return lhs.number < rhs.number;
    }




    /*************************************************
    ********************   Stop   ********************
    **************************************************/

    Stop::Stop()
        : stop_name("")
        , coordinates()
        , distance_to_stops()
    {
    }


    Stop::Stop(const Stop& other)
        : stop_name(other.stop_name)
        , coordinates(other.coordinates)
        , distance_to_stops(other.distance_to_stops)
    {
    }


    Stop& Stop::operator=(const Stop& rhs)
    {
        if (this == &rhs)
            return *this;

        stop_name = rhs.stop_name;
        coordinates = rhs.coordinates;
        distance_to_stops = rhs.distance_to_stops;

        return *this;
    }


    inline bool operator==(const Stop& lhs, const Stop& rhs)
    {
        return lhs.stop_name == rhs.stop_name;
    }




    /**************************************************
    ********************   Route   ********************
    ***************************************************/

    size_t Route::CalculateRouteLength() const
    {
        if (route_stops.size() < 1)
            return 0;

        size_t route_length = 0;


        for (int32_t i = 1; i < route_stops.size(); i++)
        {
            const std::string& next_stop_name = route_stops.at(i)->stop_name;

            if (route_stops.at(i - 1)->distance_to_stops.count(next_stop_name))
            {
                route_length += route_stops.at(i - 1)->distance_to_stops.at(next_stop_name);
            }
            else if (i - 1 >= 0)
            {
                route_length += route_stops.at(i)->distance_to_stops.at(route_stops.at(i - 1)->stop_name);
            }
        }

        if (!circular_route)
        {
            for (int32_t i = static_cast<int>(route_stops.size()) - 1; i > 0; i--)
            {
                const std::string& next_stop_name = route_stops.at(i - 1)->stop_name;

                if (route_stops.at(i)->distance_to_stops.count(next_stop_name))
                {
                    route_length += route_stops.at(i)->distance_to_stops.at(next_stop_name);
                }
                else if (i - 1 >= 0)
                {
                    route_length += route_stops.at(i - 1)->distance_to_stops.at(route_stops.at(i)->stop_name);
                }
            }
        }

        return route_length;
    }


    double Route::CalculateCurvatureRoute() const
    {
        if (route_stops.size() < 1)
            return 0.0;

        double      route_distance = 0.0;
        size_t      route_length = CalculateRouteLength();
        Coordinates from = route_stops.at(0)->coordinates;
        Coordinates to;

        for (size_t i = 1; i < route_stops.size(); i++)
        {
            to = route_stops.at(i)->coordinates;
            route_distance += ComputeDistance(from, to);
            from = to;
        }
        if (!circular_route)
        {
            route_distance *= 2;
        }


        return route_length / route_distance;
    }

    


    /****************************************************
    ********************   Request   ********************
    *****************************************************/

    Request::Request()
        : request_type_(RequestType::UNKNOWN)
        , circular_route(false)
        , route_()
        , bus_(nullptr)
        , stop_(nullptr)
    {
    }


    Request::Request(std::string_view request_str)
        : request_type_(RequestType::UNKNOWN)
        , route_()
        , bus_(nullptr)
        , stop_(nullptr)
    {
        ParseRequest(request_str);
    }


    Request::~Request()
    {
        Clear();
    }


    void Request::Clear()
    {
        if (stop_ != nullptr)
            delete stop_;
        else if (bus_ != nullptr)
            delete bus_;
        else if (stop_ != nullptr)
            route_.clear();
        else
        {
        }
    }


    const RequestType& Request::GetRequestType() const
    {
        return request_type_;
    }


    bool Request::GetIsCircularRoute() const
    {
        return circular_route;
    }


    const std::vector<Stop>& Request::GetRoute() const
    {
        return route_;
    }


    const Bus* Request::GetBus() const
    {
        return bus_;
    }


    const Stop* Request::GetStop() const
    {
        return stop_;
    }



    void Request::ParseRequest(std::string_view request_str)
    {
        if (request_str.empty())
            throw std::invalid_argument("ParseRequests: Empty request!");

        // Определяем какой тип запроса
        request_type_ = ParseRequestType(request_str);

        // Парсим запрос, в зависимости от его типа
        if (request_type_ == RequestType::ADD_ROUTE)
        {
            ParseAddRouteRequest(request_str);
        }
        else if (request_type_ == RequestType::ADD_STOP)
        {
            ParseAddStopRequest(request_str);
        }
        else if (request_type_ == RequestType::ROUTE_INFO)
        {
            request_str.remove_prefix(4);
            bus_ = new Bus{ std::string{ ReadWord(request_str, '\n') } };
        }
        else if (request_type_ == RequestType::STOP_INFO)
        {
            stop_ = new Stop();
            request_str.remove_prefix(5);
            stop_->stop_name = ReadWord(request_str, '\n');
        }
    }


    std::string_view Request::ReadWord(std::string_view& str, const char end_char)
    {
        if (str.size() == 0)
            return str;

        std::string_view out = str;
        bool need_reduction = false;

        for (size_t i = 0; i < str.size(); i++)
        {
            if (out.at(i) == end_char)
            {
                out.remove_suffix(out.size() - i);
                need_reduction = true;
                break;
            }
        }

        if (end_char == ' ' && need_reduction)
        {
            str.remove_prefix(out.size() + 1);
        }
        else if ((end_char == '>' || end_char == '-') && need_reduction)
        {
            out.remove_suffix(1);
            str.remove_prefix(out.size() + 3);
        }
        else if (end_char == 'm')
        {
            str.remove_prefix(out.size() + 5);
        }
        else if (need_reduction)
        {
            if (out.size() + 2 > str.size())
                str.remove_prefix(out.size() + 1);
            else
                str.remove_prefix(out.size() + 2);
        }
        else
        {
            str.remove_prefix(out.size());
        }

        return out;
    }


    RequestType Request::ParseRequestType(std::string_view request_str) const
    {
        RequestType request_type = RequestType::UNKNOWN;

        std::string_view word = ReadWord(request_str);

        if (word == "Bus")
        {
            // Проверяем последний символ - есть ':' или нет
            if (request_str.find(':') != std::string_view::npos)
            {
                word = ReadWord(request_str, ':');
                request_type = RequestType::ADD_ROUTE;
            }
            else
            {
                word = ReadWord(request_str, '\n');
                request_type = RequestType::ROUTE_INFO;
            }
        }
        else if (word == "Stop")
        {
            if (request_str.find(':') != std::string_view::npos)
            {
                request_type = RequestType::ADD_STOP;
            }
            else
            {
                word = ReadWord(request_str, '\n');
                request_type = RequestType::STOP_INFO;
            }
        }

        return request_type;
    }


    void Request::ParseAddRouteRequest(std::string_view request_str)
    {
        request_str.remove_prefix(4); // Пропускаем символы "Bus " в запросе

        // Читаем номер автобуса
        std::string_view word = ReadWord(request_str, ':');
        bus_ = new Bus{ std::string{ word } };

        circular_route = request_str.find('>') != std::string_view::npos;
        char delimiter = (circular_route) ? '>' : '-';

        while (request_str.size() != 0)
        {
            Stop stop;
            word = ReadWord(request_str, delimiter);
            stop.stop_name = std::string{ word };
            route_.push_back(stop);
        }
    }


    void Request::ParseAddStopRequest(std::string_view request_str)
    {
        request_str.remove_prefix(5); // Пропускаем символы "Stop " в запросе

        stop_ = new Stop();

        // Читаем и записываем название остановки
        std::string_view word = ReadWord(request_str, ':');
        stop_->stop_name = std::string{ word };

        word = ReadWord(request_str, ',');
        bool is_distance_list = request_str.find(',') != std::string_view::npos;

        if (is_distance_list)
        {
            stop_->coordinates.lat = std::stod(std::string{ word });
            word = ReadWord(request_str, ',');
            stop_->coordinates.lng = std::stod(std::string{ word });

            std::vector<std::string_view> info_for_distance;
            while (request_str.size() != 0)
            {
                if (request_str.find(',') != std::string_view::npos)
                {
                    word = ReadWord(request_str, ',');
                    info_for_distance.push_back(word);
                }
                else
                {
                    word = ReadWord(request_str, '\n');
                    info_for_distance.push_back(word);
                }
            }

            for (std::string_view& info : info_for_distance)
            {
                size_t distance = std::stoi(std::string{ ReadWord(info, 'm') });
                stop_->distance_to_stops[std::string{ info }] = distance;
            }
        }
        else
        {
            stop_->coordinates.lat = std::stod(std::string{ word });
            stop_->coordinates.lng = std::stod(std::string{ request_str });
        }
    }




    /***************************************************************
    ********************   TransportCatalogue   ********************
    ****************************************************************/

    Responce TransportCatalogue::GetRequest(const Request& request)
    {
        Responce responce = nullptr;

        if (request.GetRequestType() == RequestType::ADD_ROUTE)
        {
            AddBus(*request.GetBus(), request.GetRoute(), request.GetIsCircularRoute());
        }
        else if (request.GetRequestType() == RequestType::ADD_STOP)
        {
            AddStop(*request.GetStop());
        }
        else if (request.GetRequestType() == RequestType::ROUTE_INFO)
        {
            auto info = GetBusInfo(request.GetBus()->number);

            if (info.has_value())
                responce = info.value();
            else
                responce = std::string("Bus " + request.GetBus()->number + ": not found");
        }
        else if (request.GetRequestType() == RequestType::STOP_INFO)
        {
            auto info = GetStopInfo(request.GetStop()->stop_name);

            if (info.has_value())
                responce = info.value();
            else
                responce = std::string("Stop " + request.GetStop()->stop_name  + ": not found");
        }

        return responce;
    }


    std::shared_ptr<Bus> TransportCatalogue::FindBus(const std::string& bus_name) const
    {
        if (buses_.count(bus_name))
            return buses_.at(bus_name);
        else
            return nullptr;
    }


    std::shared_ptr<Stop> TransportCatalogue::FindStop(const std::string& stop_name) const
    {
        if (stops_.count(stop_name))
            return stops_.at(stop_name);
        else
            return nullptr;
    }


    void TransportCatalogue::AddStop(const Stop& stop)
    {
        // Если находим
        if (stops_.count(stop.stop_name))
        {
            stops_.at(stop.stop_name)->coordinates = stop.coordinates;
            stops_.at(stop.stop_name)->distance_to_stops = stop.distance_to_stops;
        }
        else
        {
            stops_.emplace(stop.stop_name, std::make_shared<Stop>(stop));
        }
    }


    void TransportCatalogue::AddBus(const Bus& bus, const std::vector<Stop>& stops, bool circular_route)
    {
        Route route;

        // заполняем остановки маршрута
        for (const Stop& stop : stops)
        {
            if (stops_.count(stop.stop_name))
            {
                // если находим то добавляем указатель на остановку в маршрут
                route.route_stops.push_back(stops_.at(stop.stop_name));
            }
            else
            {
                // если не находим, то добавляем остановку в базу
                stops_.emplace(stop.stop_name, std::make_shared<Stop>(stop));
                route.route_stops.push_back(stops_.at(stop.stop_name));
            }
        }

        // круговой маршрут или нет
        route.circular_route = circular_route;

        // добавляем маршрут в каталог
        Bus tmp_bus{ bus.number, std::make_shared<Route>(std::move(route)) };
        buses_[tmp_bus.number] = std::make_shared<Bus>(tmp_bus);
    }


    std::optional<BusInfo> TransportCatalogue::GetBusInfo(const std::string& bus_name) const
    {
        const std::shared_ptr<Bus> answer(FindBus(bus_name));
        if (answer == nullptr)
            return std::nullopt;

        const Route& route = *answer.get()->bus_route;
        BusInfo out_info;

        if (route.route_stops.size() == 0)
            return out_info;

        out_info.bus_number = buses_.at(bus_name)->number;

        out_info.route_length = route.CalculateRouteLength();

        out_info.stops_on_route =
            (route.circular_route) ? route.route_stops.size() : route.route_stops.size() * 2 - 1;

        out_info.unique_stops =
            std::set(route.route_stops.begin(), route.route_stops.end()).size();

        out_info.curvature = route.CalculateCurvatureRoute();

        return out_info;
    }


    std::optional<StopInfo> TransportCatalogue::GetStopInfo(const std::string& stop_name) const
    {
        const auto answer = FindStop(stop_name);
        if (answer == nullptr)
            return std::nullopt;

        const Stop& stop = *answer.get();
        StopInfo out_info;

        out_info.stop_name = stop_name;

        // Названия должны быть отсортированы в алфавитном порядке
        for (const auto[_, bus] : buses_)
        {
            const auto it = std::find_if(bus->bus_route->route_stops.begin(),
                bus->bus_route->route_stops.end(),
                [&stop_name](const std::shared_ptr<Stop>& stop_ptr)
                { return stop_ptr.get()->stop_name == stop_name; });

            if (it != bus->bus_route->route_stops.end())
                out_info.bus_numbers.insert(bus->number);

        }

        return out_info;
    }
}