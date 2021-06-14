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

//struct     Transport;           // Описание транспорта
//struct     TransportHasher      // Хешер структуры Transport
// 
//struct     Stop;                // Описание остановки
//struct     StopHasher           // Хешер структуры Stop
// 
//struct     Route;               // Описание маршрута транспорта
// 
//enum class RequestType;         // Типы запросов к базе данных
//class      Request;             // Запрос в базу
// 
//enum class ResponceType;        // Типы ответа от базы данных
// 
//struct     RouteInfo;           // Структура, хранящая информацию о маршруте для вывода
//struct     StopInfo;            // Структура, хранящая информацию о остановке для вывода
// 
//struct     Responce;            // Ответ базы данных на запрос
// 
//class      TransportCatalogue;  // Транспортный справочник



struct Transport
{
    std::string number;
};

inline bool operator==(const Transport& lhs, const Transport& rhs);

inline bool operator<(const Transport& lhs, const Transport& rhs);


struct TransportHasher
{
    std::hash<std::string> hasher;

    size_t operator()(const Transport& transport) const;
};


struct Stop
{
    std::string                   stop_name;
    Coordinates                   coordinates;
    std::set<Transport>           transports;
    std::map<std::string, size_t> distance_to_stops;

    Stop();

    Stop(const std::string& stop_name, const Coordinates& coordinates);

    Stop(const Stop& other);

    Stop& operator=(const Stop& rhs);
};

inline bool operator==(const Stop& lhs, const Stop& rhs);

//struct StopHasher
//{
//    size_t operator()(const Stop& stop) const;
//};


struct Route
{
    std::deque<const Stop*> route_stops;
    bool                    circular_route = false;

    size_t CalculateRouteLength() const;

    double CalculateCurvatureRoute() const;
};


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

    const Transport* GetTransport() const;

    const Stop* GetStop() const;

    void ParseRequest(std::string_view request_str);

    static std::string_view ReadWord(std::string_view& str, const char end_char = ' ');

private:
    RequestType request_type;

    // От request_type зависит какие структуры заполнены
    bool              circular_route;
    std::vector<Stop> route_;
    Transport*        transport_;
    Stop*             stop_;
};



enum class ResponceType
{
    EMPTY = 0,
    ROUTE_INFO,
    STOP_INFO
};

struct RouteInfo
{
    Transport transport_info;
    size_t    stops_on_route = 0;
    size_t    unique_stops   = 0;
    size_t    route_length   = 0;
    double    curvature      = 0;
    bool      is_exists      = false;
};

struct StopInfo
{
    std::string_view           stop_name  = "";
    const std::set<Transport>* transports =  nullptr;
    bool                       is_exists  = false;
};

struct Responce
{
    ResponceType responce_type = ResponceType::EMPTY;
    RouteInfo    route_info;
    StopInfo     stop_info;
};



class TransportCatalogue
{
public:
    Responce GetRequest(const Request& request);

    std::optional<const Route*> FindRoute(const std::string_view route_name) const;

    std::optional<const Stop*> FindStop(const std::string_view stop_name) const;

private:
    std::unordered_map<Transport, Route, TransportHasher> routes_;
    std::deque<Stop> stops_;


    void AddStop(const Stop& stop);

    void AddRoute(const Transport& transport, const std::vector<Stop>& stops, bool circular_route = false);

    std::optional<RouteInfo> GetRouteInfo(const std::string_view route_name) const;

    std::optional<StopInfo> GetStopInfo(const std::string_view stop_name) const;
};
