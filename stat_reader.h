#pragma once

#include "transport_catalogue.h"
#include <iostream>
#include <string_view>
#include <iomanip>


// Функция вывода ответа из справочника в поток ввода/вывода.
void PrintRouteInfo(std::ostream& out, const Responce& responce)
{
    const RouteInfo& out_info = responce.route_info;

    if (responce.responce_type == ResponceType::EMPTY)
    {
        out << "Bus " << responce.route_info.transport_info.number << ": "
            << "not found" << std::endl;
    }
    else
    {
        out << out_info.transport_info.number << ": "
            << out_info.stops_on_route << " stops on route, "
            << out_info.unique_stops << " unique stops, "
            << std::setprecision(6) << out_info.route_length
            << " route length" << std::endl;
    }
}

void ReadOutRequest(TransportCatalogue& transport_catalogue, const std::string_view request_words)
{
    Request request(request_words);
    PrintRouteInfo(std::cout, transport_catalogue.GetRequest(request));
}