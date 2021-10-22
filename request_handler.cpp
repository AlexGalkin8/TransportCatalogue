#include "request_handler.h"

#include <algorithm>
#include <variant>

using namespace transport_catalogue::objects;
using namespace std;

namespace transport_catalogue::request_handler
{
    RequestHandler::RequestHandler(
        database::TransportCatalogue& database,
        const renderer::MapRenderer& renderer,
        const router::TransportRouter& router,
        const serialization::Serialization& serializator)
        : database_(database)
        , renderer_(renderer)
        , router_(router)
        , serializator_(serializator)
    {
    }


    Response RequestHandler::RequestToBase(const Request& request)
    {
        Response response;

        if (request.request_type == RequestType::ADD_BUS)
        {
            database_.AddBus(get<Bus>(request.value));
        }
        else if (request.request_type == RequestType::ADD_STOP)
        {
            database_.AddStop(get<Stop>(request.value));
        }
        else if (request.request_type == RequestType::BUS_INFO)
        {
            auto info = GetBusInfo(get<InfoRequest>(request.value));

            if (info.has_value())
            {
                response.response_type = ResponseType::BUS_INFO;
                response.value = info.value();
            }
            else
            {
                response.response_type = ResponseType::MESSAGE;
                response.value = string("Bus " + get<InfoRequest>(request.value) + ": not found");
            }
        }
        else if (request.request_type == RequestType::STOP_INFO)
        {

            auto info = GetStopInfo(get<InfoRequest>(request.value));

            if (info.has_value())
            {
                response.response_type = ResponseType::STOP_INFO;
                response.value = info.value();
            }
            else
            {
                response.response_type = ResponseType::MESSAGE;
                response.value = string("Stop " + get<InfoRequest>(request.value) + ": not found");
            }
        }

        return response;
    }


    optional<BusInfo> RequestHandler::GetBusInfo(const string& bus_name) const
    {
        const auto answer = database_.FindBus(bus_name);

        if (answer == nullptr)
            return std::nullopt;

        const Route& route = answer.get()->bus_route;
        BusInfo out_info;

        if (route.route_stops.size() == 0)
        {
            out_info.bus_number = answer->number;
            return out_info;
        }

        int route_length = 0;
        double route_distance = 0;
        const auto& stops = route.route_stops;

        for (auto it = stops.begin(); std::distance(it, stops.end()) != 1; ++it)
        {
            const auto& current_stop = *it;
            const auto& next_stop = *(it + 1);
            route_distance += geo::ComputeDistance(current_stop->coordinates, next_stop->coordinates);
            route_length += database_.GetDistanceBetweenStops(current_stop, next_stop);
        }

        if (!route.is_roundtrip)
        {
            for (auto it = stops.rbegin(); std::distance(it, stops.rend()) != 1; ++it)
            {
                const auto& current_stop = *it;
                const auto& next_stop = *(it + 1);
                route_length += database_.GetDistanceBetweenStops(current_stop, next_stop);
            }
            route_distance *= 2;
        }


        out_info.bus_number = answer->number;

        out_info.route_length = route_length;

        out_info.stops_on_route =
            (route.is_roundtrip) ? route.route_stops.size() : route.route_stops.size() * 2 - 1;

        out_info.unique_stops =
            std::set(route.route_stops.begin(), route.route_stops.end()).size();

        if (route_length != 0)
            out_info.curvature = route_length / route_distance;

        return out_info;
    }


    optional<StopInfo> RequestHandler::GetStopInfo(const string& name) const
    {
        const auto answer = database_.FindStop(name);

        if (answer == nullptr)
            return std::nullopt;

        StopInfo out_info;
        out_info.name = answer->stop_name;

        const Buses& buses = database_.GetBuses();

        // Названия должны быть отсортированы в алфавитном порядке
        for (const auto [_, bus] : buses)
        {
            const auto it = std::find_if(bus->bus_route.route_stops.begin(),
                bus->bus_route.route_stops.end(),
                [&name](const std::shared_ptr<Stop>& stop_ptr)
                { return stop_ptr.get()->stop_name == name; });

            if (it != bus->bus_route.route_stops.end())
                out_info.bus_numbers.insert(bus->number);
        }
        return out_info;
    }


    proto_renderer::Map RequestHandler::RenderMap() const
    {
        return serializator_.LoadMap();
    }


    optional<router::ReportRouter> RequestHandler::GetReportRouter(const string_view from,
        const string_view to) const
    {
        return router_.GetReportRouter(from, to);
    }

} // namespace transport_catalogue::request_handler