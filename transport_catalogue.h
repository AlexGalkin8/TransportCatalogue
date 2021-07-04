#pragma once

#include "geo.h"

#include <deque>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <optional>
#include <map>
#include <memory>
#include <functional>
#include <variant>



namespace transport_catalogue
{
    struct     Bus;                 // Описание автобуса
    struct     Stop;                // Описание остановки
    struct     Route;               // Описание маршрута транспорта
    enum class RequestType;         // Типы запросов к базе данных
    class      Request;             // Запрос в базу
    enum class ResponceType;        // Типы ответа от базы данных
    struct     BusInfo;             // Структура, хранящая информацию о маршруте для вывода
    struct     StopInfo;            // Структура, хранящая информацию о остановке для вывода
    class      TransportCatalogue;  // Транспортный справочник

    using Responce = std::variant<std::nullptr_t, BusInfo, StopInfo, std::string>;


    struct Stop
    {
        std::string                   stop_name;
        coordinate_tools::Coordinates coordinates;
        std::map<std::string, size_t> distance_to_stops;

        Stop();

        explicit Stop(const Stop& other);

        Stop& operator=(const Stop& rhs);
    };

    inline bool operator==(const Stop& lhs, const Stop& rhs);


    struct Route
    {
        std::deque<std::shared_ptr<Stop>> route_stops;
        bool circular_route = false;

        size_t CalculateRouteLength() const;

        double CalculateCurvatureRoute() const;
    };


    struct Bus
    {
        std::string number;
        std::shared_ptr<Route> bus_route;
    };

    inline bool operator==(const Bus& lhs, const Bus& rhs);

    inline bool operator<(const Bus& lhs, const Bus& rhs);


    enum class RequestType
    {
        UNKNOWN = 0,
        ADD_STOP,
        ADD_ROUTE,
        ROUTE_INFO,
        STOP_INFO
    };



    class Request
    {
    public:
        Request();

        Request(std::string_view request_str);

        ~Request();

        void Clear();

        const RequestType& GetRequestType() const;

        bool GetIsCircularRoute() const;

        const std::vector<Stop>& GetRoute() const;

        const Bus* GetBus() const;

        const Stop* GetStop() const;

        void ParseRequest(std::string_view request_str);

        static std::string_view ReadWord(std::string_view& str, const char end_char = ' ');

    private:
        RequestType ParseRequestType(std::string_view request_str) const;

        void ParseAddRouteRequest(std::string_view request_str);

        void ParseAddStopRequest(std::string_view request_str);

    private:
        RequestType request_type_;

        // От request_type_ зависит какие структуры заполнены
        bool              circular_route;
        std::vector<Stop> route_;
        Bus*              bus_;
        Stop*             stop_;
    };



    enum class ResponceType
    {
        EMPTY = 0,
        ROUTE_INFO,
        STOP_INFO
    };

    struct BusInfo
    {
        std::string_view       bus_number;
        size_t                 stops_on_route = 0;
        size_t                 unique_stops   = 0;
        size_t                 route_length   = 0;
        double                 curvature      = 0.0;
        bool                   is_exists      = false;
    };

    struct StopInfo
    {
        std::string_view           stop_name = "";
        std::set<std::string_view> bus_numbers;
    };



    class TransportCatalogue
    {
    public:
        Responce GetRequest(const Request& request);

        std::shared_ptr<Bus> FindBus(const std::string& bus_name) const;

        std::shared_ptr<Stop> FindStop(const std::string& stop_name) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<Bus>>   buses_;
        std::unordered_map<std::string, std::shared_ptr<Stop>>  stops_;


        void AddStop(const Stop& stop);

        void AddBus(const Bus& bus, const std::vector<Stop>& stops, bool circular_route = false);

        std::optional<BusInfo> GetBusInfo(const std::string& route_name) const;

        std::optional<StopInfo> GetStopInfo(const std::string& stop_name) const;
    };
}