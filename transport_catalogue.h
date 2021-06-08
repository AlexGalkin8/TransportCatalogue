#pragma once

#include "geo.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <string_view>

using namespace std::string_literals;

enum class RequestType;         // Типы запросов к базе данных
struct     Responce;            // Ответ базы данных на запрос
struct     Route;               // Описание маршрута транспорта
struct     Stop;                // Описание остановки
struct     Transport;           // Описание транспорта
class      Request;             // Запрос в базу
class      TransportCatalogue;  // Транспортный справочник


class TransportCatalogue
{
public:
    /*
    * добавление маршрута в базу,
    * добавление остановки в базу,
    * поиск маршрута по имени,
    * поиск остановки по имени,
    * получение информации о маршруте.
    */


    //void AddStop(const Stop& stop);
    //void AddRoute(const Transport& transport, const Route& route);
    //const Route& FindRoute(const std::string_view route_name)
    //const Stop& FindStop(const std::string_view stop_name)
    //string GetRouteInfo(const std::string_view route_name) const;

private:
    std::unordered_map<Transport, Route> routes_;
};


enum class RequestType
{
    UNKNOWN = 0,
    ADD_STOP,
    ADD_ROUTE,
    RESPONSE
};


class Request
{
public:
    Request()
        : stop_(nullptr)
        , transport_(nullptr)
        , route_(nullptr)
    {
    }

    /* @brief Парсинг запроса в виде строки. В дальнейшем возможно парсить другие виды запросов */
    Request ParseRequest(std::string_view request_words)
    {
        if (request_words.size() < 0)
            throw std::invalid_argument("ParseRequests: Empty request!");

    }

    void Clear()
    {
        if (stop_ != nullptr)
            delete stop_;
        else if (transport_ != nullptr)
            delete transport_;
        else if (stop_ != nullptr)
            delete route_;
        else
        { }
    }

    ~Request()
    {
        Clear();
    }

private:
    RequestType request_type;

    // От request_type зависит какие структуры заполнены
    Stop*      stop_;
    Transport* transport_;
    Route*     route_;
};


struct RouteInfo
{
    uint16_t stops_on_route;
    uint16_t unique_stops;
    uint32_t route_length;
};

struct Stop
{
    std::string stop_name;
    Coordinates coordinates;
};

struct Route
{
    std::deque<Stop> route;
    uint32_t route_length;
    bool circular_route;
};

struct Transport
{
    uint32_t number;
};


