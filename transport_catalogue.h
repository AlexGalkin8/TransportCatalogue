#pragma once

#include "geo.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <set>
#include <optional>
#include <algorithm>
#include <execution>

//enum class RequestType;         // Типы запросов к базе данных
//enum class ResponceType;
//struct     Responce;            // Ответ базы данных на запрос
//struct     Route;               // Описание маршрута транспорта
//struct     Stop;                // Описание остановки
//struct     Transport;           // Описание транспорта
//struct     RouteInfo;
//class      Request;             // Запрос в базу
//class      TransportCatalogue;  // Транспортный справочник


struct Stop
{
    Stop()
        : stop_name("")
        , coordinates()
    {
    }

    Stop(const std::string& stop_name, const Coordinates coordinates)
        :stop_name(stop_name)
        , coordinates(coordinates)
    {
    }

    Stop(const Stop& other)
        : stop_name(other.stop_name), coordinates(other.coordinates)
    {
    }

    Stop& operator=(const Stop& rhs)
    {
        if (this == &rhs)
            return *this;

        stop_name = rhs.stop_name;
        coordinates = rhs.coordinates;

        return *this;
    }

    std::string stop_name;
    Coordinates coordinates;
};

inline bool operator==(const Stop& lhs, const Stop& rhs)
{
    return lhs.stop_name == rhs.stop_name;
}

//struct StopHasher
//{
//    size_t operator()(const Stop& stop) const
//    {
//        CoordinatesHasher coordinates_hasher;
//        return std::hash<std::string>{}(stop.stop_name) +
//            std::hash<size_t>{}(coordinates_hasher(stop.coordinates));
//    }
//};


struct Route
{
    std::deque<const Stop*> route_stops;
    bool circular_route = false;

    double CalculateRouteLength() const
    {
        if (route_stops.size() < 1)
            return 0.0;

        double      route_length = 0.0;
        //bool        is_count     = false;
        Coordinates from = route_stops.at(0)->coordinates;
        Coordinates to;

        for (size_t i = 1; i < route_stops.size(); i++)
        {
            to = route_stops.at(i)->coordinates;
            route_length += ComputeDistance(from, to);
            from = to;
        }
        if (!circular_route)
        {
            route_length *= 2;
        }

        return route_length;
    }
};


struct Transport
{
    std::string number;
};

inline bool operator==(const Transport& lhs, const Transport& rhs)
{
    return lhs.number == rhs.number;
}


struct TransportHasher
{
    size_t operator()(const Transport& transport) const
    {
        return hasher(transport.number);
    }

    std::hash<std::string> hasher;
};



enum class RequestType
{
    UNKNOWN = 0,
    ADD_STOP,
    ADD_ROUTE,
    ROUTE_INFO
};

class Request
{
public:
    Request()
        : request_type(RequestType::UNKNOWN)
        , circular_route(false)
        , route_()
        , transport_(nullptr)
        , stop_(nullptr)
    {
    }

    Request(std::string_view request_str)
        : request_type(RequestType::UNKNOWN)
        , route_()
        , transport_(nullptr)
        , stop_(nullptr)
    {
        ParseRequest(request_str);
    }

    void ParseRequest(std::string_view request_str)
    {
        if (request_str.empty())
            throw std::invalid_argument("ParseRequests: Empty request!");

        std::string_view word = ReadWord(request_str);

        // Определяем какой тип запроса
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
            request_type = RequestType::ADD_STOP;


        if (request_type == RequestType::ADD_ROUTE)
        {
            transport_ = new Transport{ std::string{ word } };

            circular_route = request_str.find('>') != std::string_view::npos;
            /*std::find(request_str.cbegin(), request_str.cend(), '>') != request_str.cend()*/;
            char delimiter = (circular_route) ? '>' : '-';

            while (request_str.size() != 0)
            {
                Stop stop;
                word = ReadWord(request_str, delimiter);
                stop.stop_name = std::string{ word };
                route_.push_back(stop);
            }
        }
        else if (request_type == RequestType::ADD_STOP)
        {
            stop_ = new Stop();
            word = ReadWord(request_str, ':');
            stop_->stop_name = std::string{ word };

            word = ReadWord(request_str, ',');
            stop_->coordinates.lat = std::stod(std::string{ word });
            stop_->coordinates.lng = std::stod(std::string{ request_str });
        }
        else if (request_type == RequestType::ROUTE_INFO)
        {
            transport_ = new Transport{ std::string{ word } };
        }
    }

    const RequestType& GetRequestType() const
    {
        return request_type;
    }

    bool GetIsCircularRoute() const
    {
        return circular_route;
    }

    const std::vector<Stop>& GetRoute() const
    {
        return route_;
    }

    const Transport* GetTransport() const
    {
        return transport_;
    }

    const Stop* GetStop() const
    {
        return stop_;
    }

    void Clear()
    {
        if (stop_ != nullptr)
            delete stop_;
        else if (transport_ != nullptr)
            delete transport_;
        else if (stop_ != nullptr)
            route_.clear();
        else
        {
        }
    }

    ~Request()
    {
        Clear();
    }

    static std::string_view ReadWord(std::string_view& str, const char end_char = ' ')
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
        else if (need_reduction)
        {
            str.remove_prefix(out.size() + 2);
        }
        else
        {
            str.remove_prefix(out.size());
        }

        return out;
    }

private:
    RequestType request_type;

    // От request_type зависит какие структуры заполнены
    bool circular_route;
    std::vector<Stop> route_;
    Transport* transport_;
    Stop* stop_;
};



enum class ResponceType
{
    EMPTY = 0,
    INFO_ROUTE,
};

struct RouteInfo
{
    Transport transport_info;
    size_t stops_on_route = 0;
    size_t unique_stops = 0;
    double   route_length = 0;
};

struct Responce
{
    ResponceType responce_type = ResponceType::EMPTY;
    RouteInfo route_info;
};



class TransportCatalogue
{
public:
    Responce GetRequest(const Request& request)
    {
        if (request.GetRequestType() == RequestType::ADD_ROUTE)
        {
            AddRoute(*request.GetTransport(), request.GetRoute(), request.GetIsCircularRoute());
        }
        else if (request.GetRequestType() == RequestType::ADD_STOP)
        {
            AddStop(*request.GetStop());
        }
        else if (request.GetRequestType() == RequestType::ROUTE_INFO)
        {
            auto info = GetRouteInfo(request.GetTransport()->number);

            if (!info.has_value())
                return Responce{ ResponceType::EMPTY,
                RouteInfo{ Transport{ std::string { request.GetTransport()->number } }, 0, 0, 0 } };
            else
                return Responce{ ResponceType::INFO_ROUTE, info.value() };
        }

        return Responce();
    }

    std::optional<const Route*> FindRoute(const std::string_view route_name) const
    {
        auto it = routes_.find(Transport{ std::string{ route_name } });

        if (it == routes_.end())
            return std::nullopt;
        else
            return &it->second;
    }

    std::optional<const Stop*> FindStop(const std::string_view stop_name) const
    {
        std::find(std::execution::par, stops_.begin(), stops_.end(), Stop{ static_cast<std::string>(stop_name), Coordinates() });
        auto it = std::find_if(std::execution::par, stops_.begin(), stops_.end(), [&stop_name]
        (const Stop& stop) { return stop.stop_name == static_cast<std::string>(stop_name); });

        if (it == stops_.end())
            return std::nullopt;
        else
            return &(*it); // В optional нельзя передавать ссылку, поэтому пользуемся указателями
    }

private:
    std::unordered_map<Transport, Route, TransportHasher> routes_;
    std::vector<Stop> stops_;
    //std::unordered_set<Stop, StopHasher> stops_; // при работе с этим контейнером не работает AddStop usd: Уже вообще ничего

    void AddStop(const Stop& stop)
    {
        // ищем в данные об остановках переданную остановку
        auto it = std::find(stops_.begin(), stops_.end(), stop);

        // Если находим
        if (it != stops_.end())
            *it = stop;
        else
            stops_.push_back(stop);
    }

    void AddRoute(const Transport& transport, const std::vector<Stop>& stops, bool circular_route=false)
    {
        Route route;
        for (const Stop& stop : stops)
        {
            // ищем остановку в базе остановок
            auto it = FindStop(stop.stop_name);

            if (it.has_value())
            {
                // если находим то добавляем указатель на остановку в маршрут
                route.route_stops.push_back(it.value());
            }
            else
            {
                // если не находим, то добавляем остановку в базу
                stops_.push_back(stop);
                route.route_stops.push_back(&stops_.at(stops_.size() - 1));
            }
        }

        // круговой маршрут или нет
        route.circular_route = circular_route;
        //route.circular_route = *route.route_stops.begin() == *(route.route_stops.end() - 1);

        // добавляем новый маршрут или меняем старый
        routes_[transport] = route;
    }

    std::optional<RouteInfo> GetRouteInfo(const std::string_view route_name)
    {
        const auto answer = FindRoute(route_name);
        if (!answer.has_value())
            return std::nullopt;

        const Route& route = *answer.value();
        RouteInfo    out_info;

        out_info.transport_info = Transport{ std::string { route_name } };
        out_info.route_length = route.CalculateRouteLength();
        out_info.stops_on_route =
            (route.circular_route) ? route.route_stops.size() : route.route_stops.size() * 2 - 1;
        out_info.unique_stops =
            std::set<const Stop*>(route.route_stops.begin(), route.route_stops.end()).size();

        return out_info;
    }
};



