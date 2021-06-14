#include "transport_catalogue.h"

#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <utility>



inline bool operator==(const Transport& lhs, const Transport& rhs)
{
    return lhs.number == rhs.number;
}


inline bool operator<(const Transport& lhs, const Transport& rhs)
{
    return lhs.number < rhs.number;
}


size_t TransportHasher::operator()(const Transport& transport) const
{
    return hasher(transport.number);
}


Stop::Stop()
    : stop_name("")
    , coordinates()
    , transports()
{
}


Stop::Stop(const std::string& stop_name, const Coordinates& coordinates)
    :stop_name(stop_name)
    , coordinates(coordinates)
{
}


Stop::Stop(const Stop& other)
    : stop_name(other.stop_name)
    , coordinates(other.coordinates)
    , transports(other.transports)
    , distance_to_stops(other.distance_to_stops)
{
}


Stop& Stop::operator=(const Stop& rhs)
{
    if (this == &rhs)
        return *this;

    stop_name = rhs.stop_name;
    coordinates = rhs.coordinates;
    transports = rhs.transports;
    distance_to_stops = rhs.distance_to_stops;

    return *this;
}


inline bool operator==(const Stop& lhs, const Stop& rhs)
{
    return lhs.stop_name == rhs.stop_name;
}


//size_t StopHasher::operator()(const Stop& stop) const
//{
//    CoordinatesHasher coordinates_hasher;
//    return std::hash<std::string>{}(stop.stop_name) +
//           std::hash<size_t>{}(coordinates_hasher(stop.coordinates));
//}


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


Request::Request()
    : request_type(RequestType::UNKNOWN)
    , circular_route(false)
    , route_()
    , transport_(nullptr)
    , stop_(nullptr)
{
}


Request::Request(std::string_view request_str)
    : request_type(RequestType::UNKNOWN)
    , route_()
    , transport_(nullptr)
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
    else if (transport_ != nullptr)
        delete transport_;
    else if (stop_ != nullptr)
        route_.clear();
    else
    {
    }
}


const RequestType& Request::GetRequestType() const
{
    return request_type;
}


bool Request::GetIsCircularRoute() const
{
    return circular_route;
}


const std::vector<Stop>& Request::GetRoute() const
{
    return route_;
}


const Transport* Request::GetTransport() const
{
    return transport_;
}


const Stop* Request::GetStop() const
{
    return stop_;
}


void Request::ParseRequest(std::string_view request_str)
{
    if (request_str.empty())
        throw std::invalid_argument("ParseRequests: Empty request!");

    std::string_view word = ReadWord(request_str);

    // ќпредел€ем какой тип запроса
    if (word == "Bus")
    {
        // ѕровер€ем последний символ - есть ':' или нет
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


    if (request_type == RequestType::ADD_ROUTE)
    {
        transport_ = new Transport{ std::string{ word } };
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
    else if (request_type == RequestType::ADD_STOP)
    {
        stop_ = new Stop();
        word = ReadWord(request_str, ':');
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
    else if (request_type == RequestType::ROUTE_INFO)
    {
        transport_ = new Transport{ std::string{ word } };
    }
    else if (request_type == RequestType::STOP_INFO)
    {
        stop_ = new Stop();
        stop_->stop_name = word;
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


Responce TransportCatalogue::GetRequest(const Request& request)
{
    Responce responce;

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
        responce.responce_type = ResponceType::ROUTE_INFO;

        if (info.has_value())
        {
            responce.route_info = info.value();
            responce.route_info.is_exists = true;
        }
        else
        {
            responce.route_info.is_exists = false;
            responce.route_info.transport_info.number = request.GetTransport()->number;
        }
    }
    else if (request.GetRequestType() == RequestType::STOP_INFO)
    {
        auto info = GetStopInfo(request.GetStop()->stop_name);
        responce.responce_type = ResponceType::STOP_INFO;

        if (info.has_value())
        {
            responce.stop_info = info.value();
            responce.stop_info.is_exists = true;
        }
        else
        {
            responce.stop_info.is_exists = false;
            responce.stop_info.stop_name = request.GetStop()->stop_name;
        }
    }

    return responce;
}


std::optional<const Route*> TransportCatalogue::FindRoute(const std::string_view route_name) const
{
    auto it = routes_.find(Transport{ std::string{ route_name } });
    //auto it = std::find(std::execution::par, routes_.begin(), routes_.end(), Transport{ static_cast<std::string>(route_name ) });
    if (it == routes_.end())
        return std::nullopt;
    else
        return &it->second;
}


std::optional<const Stop*> TransportCatalogue::FindStop(const std::string_view stop_name) const
{
    auto it = std::find_if(stops_.begin(), stops_.end(), [&stop_name]
    (const Stop& stop) { return stop.stop_name == static_cast<std::string>(stop_name); });

    if (it == stops_.end())
        return std::nullopt;
    else
        return &(*it); // ¬ optional нельз€ передавать ссылку, поэтому пользуемс€ указател€ми
}


void TransportCatalogue::AddStop(const Stop& stop)
{
    // ищем в данные об остановках переданную остановку
    auto it = std::find(stops_.begin(), stops_.end(), stop);

    // ≈сли находим
    if (it != stops_.end())
    {
        it->coordinates = stop.coordinates;
        it->distance_to_stops = stop.distance_to_stops;
    }
    else
        stops_.push_back(stop);
}


void TransportCatalogue::AddRoute(const Transport& transport, const std::vector<Stop>& stops, bool circular_route)
{
    Route route;
    for (const Stop& stop : stops)
    {
        // ищем остановку в базе остановок
        auto it = std::find(stops_.begin(), stops_.end(), stop);

        if (it != stops_.end())
        {
            // если находим то добавл€ем указатель на остановку в маршрут
            route.route_stops.push_back(&(*it));
            // ƒобавл€ем транспорт в список транспортов, проход€щих через остановку
            it->transports.insert(transport);
        }
        else
        {
            // если не находим, то добавл€ем остановку в базу
            stops_.push_back(stop);
            stops_.at(stops_.size() - 1).transports.insert(transport);
            route.route_stops.push_back(&stops_.at(stops_.size() - 1));
        }

    }

    // круговой маршрут или нет
    route.circular_route = circular_route;

    // добавл€ем новый маршрут или мен€ем старый
    routes_[transport] = route;
}


std::optional<RouteInfo> TransportCatalogue::GetRouteInfo(const std::string_view route_name) const
{
    const auto answer = FindRoute(route_name);
    if (!answer.has_value())
        return std::nullopt;

    const Route& route = *answer.value();
    RouteInfo out_info;

    if (route.route_stops.size() == 0)
    {
        return out_info;
    }

    out_info.transport_info = Transport{ std::string { route_name } };
    out_info.route_length = route.CalculateRouteLength();
    out_info.stops_on_route =
        (route.circular_route) ? route.route_stops.size() : route.route_stops.size() * 2 - 1;
    out_info.unique_stops =
        std::set<const Stop*>(route.route_stops.begin(), route.route_stops.end()).size();
    out_info.curvature = route.CalculateCurvatureRoute();

    return out_info;
}


std::optional<StopInfo> TransportCatalogue::GetStopInfo(const std::string_view stop_name) const
{
    const auto answer = FindStop(stop_name);
    if (!answer.has_value())
        return std::nullopt;

    const Stop& stop = *answer.value();
    StopInfo out_info;

    out_info.stop_name = stop_name;
    out_info.transports = &stop.transports;

    return out_info;
}