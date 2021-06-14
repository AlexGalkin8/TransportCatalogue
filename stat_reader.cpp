#include "stat_reader.h"
#include <iomanip>


void PrintRouteInfo(std::ostream& out, const Responce& responce)
{
    const RouteInfo& out_info = responce.route_info;

    if (!out_info.is_exists)
    {
        out << "Bus " << responce.route_info.transport_info.number << ": "
            << "not found" << std::endl;
    }
    else if (out_info.stops_on_route == 0)
    {
        out << "Bus " << responce.route_info.transport_info.number << ": "
            << "no stops" << std::endl;
    }
    else
    {
        out << "Bus " << out_info.transport_info.number << ": "
            << out_info.stops_on_route << " stops on route, "
            << out_info.unique_stops << " unique stops, "
            << out_info.route_length << " route length, "
            << std::setprecision(6) << out_info.curvature
            << " curvature" << std::endl;
    }
}

void PrintStopInfo(std::ostream& out, const Responce& responce)
{
    const StopInfo& out_info = responce.stop_info;

    if (!out_info.is_exists)
    {
        out << "Stop " << out_info.stop_name << ": "
            << "not found" << std::endl;
    }
    else if (out_info.transports == nullptr || out_info.transports->size() == 0)
    {
        out << "Stop " << out_info.stop_name << ": "
            << "no buses" << std::endl;
    }
    else
    {
        out << "Stop " << out_info.stop_name << ": buses";
        for (const auto transport : *out_info.transports)
            out << " " << transport.number;
        out << std::endl;
    }
}

void ReadOutRequest(TransportCatalogue& transport_catalogue, std::istream& is)
{
    size_t num_request = 0;
    std::string request_words;
    std::getline(is, request_words);
    num_request = std::stoi(request_words);

    for (size_t i = 0; i < num_request; i++)
    {
        std::getline(is, request_words);
        Request request(request_words);
        Responce responce = transport_catalogue.GetRequest(request);
        if (responce.responce_type == ResponceType::ROUTE_INFO)
        {
            PrintRouteInfo(std::cout, responce);
        }
        else if (responce.responce_type == ResponceType::STOP_INFO)
        {
            PrintStopInfo(std::cout, responce);
        }
    }
}

void ReadOutRequest(TransportCatalogue& transport_catalogue, std::stringstream& is)
{
    std::string request_words;
    while (std::getline(is, request_words))
    {
        Request request(request_words);
        Responce responce = transport_catalogue.GetRequest(request);
        if (responce.responce_type == ResponceType::ROUTE_INFO)
        {
            PrintRouteInfo(std::cout, responce);
        }
        else if (responce.responce_type == ResponceType::STOP_INFO)
        {
            PrintStopInfo(std::cout, responce);
        }
    }
}