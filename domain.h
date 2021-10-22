#pragma once

#include "geo.h"

#include <cstdint>
#include <vector>
#include <map>
#include <variant>
#include <string_view>
#include <string>
#include <set>
#include <utility>
#include <memory>
#include <unordered_map>


namespace transport_catalogue::objects
{
    struct     Bus;                 // Описание автобуса
    struct     Stop;                // Описание остановки
    struct     Route;               // Описание маршрута транспорта
    enum class RequestType;         // Типы запросов к базе данных
    enum class ResponseType;        // Типы ответа от базы данных
    struct     BusInfo;             // Структура, хранящая информацию о маршруте для вывода
    struct     StopInfo;            // Структура, хранящая информацию о остановке для вывода
    struct     Request;             // Запрос в БД
    struct     Response;            // Ответ БД


    struct Stop
    {
        std::string                   stop_name;
        geo::Coordinates              coordinates;
        bool                          free = true;
    };


    struct Route
    {
        std::vector<std::shared_ptr<Stop>> route_stops;
        bool                              is_roundtrip = true; // true - по кругу | false - едет туда и обратно
    };


    struct Bus
    {
        std::string number;
        Route       bus_route;
    };

    struct Road
    {
        std::string bus_number = "";
        double      minutes = 0;
        int         span_count = 0;
    };


    enum class RequestType
    {
        UNKNOWN = 0,
        ADD_STOP,
        ADD_BUS,
        BUS_INFO,
        STOP_INFO
    };


    enum class ResponseType
    {
        EMPTY = 0,
        BUS_INFO,
        STOP_INFO,
        MESSAGE
    };


    struct BusInfo
    {
        std::string_view bus_number;
        int64_t          stops_on_route = 0;
        int64_t          unique_stops = 0;
        int64_t          route_length = 0;
        double           curvature = 0.0;
    };


    struct StopInfo
    {
        std::string_view           name = "";
        std::set<std::string_view> bus_numbers;
    };

    struct HashPairStops
    {
    public:
        size_t operator() (const std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>& pair) const;
    private:
        std::hash<void*> p_hasher;
    };

    using InfoRequest = std::string;
    using RequestValue = std::variant<std::nullptr_t, Bus, Stop, InfoRequest>;
    using ResponseValue = std::variant<std::nullptr_t, BusInfo, StopInfo, std::string>;

    // <название остановки, <название остановки до которой проложен маршрут, длина маршрута>>
    using StopsDistance = std::unordered_map<std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>, size_t, HashPairStops>;

    using Buses = std::map<std::string, std::shared_ptr<objects::Bus>>;
    using Stops = std::map<std::string, std::shared_ptr<objects::Stop>>;


    struct Request
    {
        RequestType  request_type = RequestType::UNKNOWN;
        RequestValue value;
    };


    struct Response
    {
        ResponseType  response_type = ResponseType::EMPTY;
        ResponseValue value;
    };


    bool operator==(const Bus& lhs, const Bus& rhs);
    bool operator<(const Bus& lhs, const Bus& rhs);
    bool operator==(const Stop& lhs, const Stop& rhs);
    bool operator>(const Road& lhs, const Road& rhs);
    bool operator<(const Road& lhs, const Road& rhs);
    Road operator+(const Road& lhs, const Road& rhs);

} // namespace transport_catalogue::objects