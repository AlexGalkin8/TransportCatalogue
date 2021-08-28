#include "request_handler.h"

#include <algorithm>

using namespace transport_catalogue::objects;


namespace transport_catalogue
{
    namespace request_handler
    {
        RequestHandler::RequestHandler(
            database::DataBase& base_data,
            const renderer::MapRenderer& renderer,
            const router::TransportRouter& router)
            : database_(base_data)
            , renderer_(renderer)
            , router_(router)
        {
        }


        Response RequestHandler::RequestToBase(const Request& request)
        {
            Response responce;

            if (request.request_type == RequestType::ADD_BUS)
            {
                database_.AddBus(std::get<Bus>(request.value));
            }
            else if (request.request_type == RequestType::ADD_STOP)
            {
                database_.AddStop(std::get<Stop>(request.value));
            }
            else if (request.request_type == RequestType::BUS_INFO)
            {
                auto info = GetBusInfo(std::get<InfoRequest>(request.value));

                if (info.has_value())
                {
                    responce.responce_type = ResponseType::BUS_INFO;
                    responce.value = info.value();
                }
                else
                {
                    responce.responce_type = ResponseType::MESSAGE;
                    responce.value = std::string("Bus " + std::get<InfoRequest>(request.value) + ": not found");
                }
            }
            else if (request.request_type == RequestType::STOP_INFO)
            {

                auto info = GetStopInfo(std::get<InfoRequest>(request.value));

                if (info.has_value())
                {
                    responce.responce_type = ResponseType::STOP_INFO;
                    responce.value = info.value();
                }
                else
                {
                    responce.responce_type = ResponseType::MESSAGE;
                    responce.value = std::string("Stop " + std::get<InfoRequest>(request.value) + ": not found");
                }
            }

            return responce;
        }


        std::optional<BusInfo> RequestHandler::GetBusInfo(const std::string& bus_name) const
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


        std::optional<StopInfo> RequestHandler::GetStopInfo(const std::string& name) const
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


        svg::Document RequestHandler::RenderMap() const
        {
            return renderer_.GetDocument();
        }


        std::optional<router::ReportRouter> RequestHandler::GetReportRouter(const std::string_view from,
            const std::string_view to) const
        {
            return router_.GetReportRouter(from, to);
        }

    } // namespace request_handler

} // namespace transport_catalogue