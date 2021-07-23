#pragma once

#include <deque>
#include <vector>
#include <map>
#include <variant>
#include <string_view>
#include <string>
#include <set>
#include <memory>
#include <unordered_map>

#include "geo.h"


namespace transport_catalogue
{
    namespace objects
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
            std::deque<std::shared_ptr<Stop>> route_stops;
            bool                              is_roundtrip = false;
        };


        struct Bus
        {
            std::string number;
            Route       bus_route;
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
            std::string_view           stop_name = "";
            std::set<std::string_view> bus_numbers;
        };


        using InfoRequest = std::string;
        using RequestValue = std::variant<std::nullptr_t, Bus, Stop, InfoRequest>;
        using ResponceValue = std::variant<std::nullptr_t, BusInfo, StopInfo, std::string>;

        using Buses = std::map<std::string, std::shared_ptr<objects::Bus>>;
        using Stops = std::map<std::string, std::shared_ptr<objects::Stop>>;

        using StopsDistance = std::unordered_map<std::string, std::shared_ptr<std::map<std::string, size_t>>>;


        struct Request
        {
            RequestType  request_type = RequestType::UNKNOWN;
            RequestValue value;
        };


        struct Response
        {
            ResponseType  responce_type = ResponseType::EMPTY;
            ResponceValue value;
        };


        inline bool operator==(const Bus& lhs, const Bus& rhs);
        inline bool operator<(const Bus& lhs, const Bus& rhs);
        inline bool operator==(const Stop& lhs, const Stop& rhs);
    } // namespace objects



    namespace database
    {
        class DataBase
        {
        public:
            const std::shared_ptr<objects::Bus> FindBus(const std::string& bus_name) const;

            const std::shared_ptr<objects::Stop> FindStop(const std::string& stop_name) const;

            const objects::Buses& GetBuses() const;

            const objects::Stops& GetStops() const;

            void AddStop(const objects::Stop& stop);

            void AddBus(const objects::Bus& bus);

            void AddDistanceBetweenStops(std::string& stop_name, std::map<std::string, size_t>& distances_for_stop);

            const std::shared_ptr<std::map<std::string, size_t>> GetDistanceBetweenStops(std::string& stop_name) const;

        private:
            objects::Buses buses_;
            objects::Stops stops_;

            objects::StopsDistance length_between_stops_;
        };
    } // namespace database

} // namespace transport_catalogue