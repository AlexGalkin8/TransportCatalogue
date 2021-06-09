#pragma once

#include "transport_catalogue.h"
#include <string_view>
#include <algorithm>


void ReadInputRequest(TransportCatalogue& transport_catalogue, const std::string_view request_words)
{
    Request request(request_words);
    transport_catalogue.GetRequest(request);
}


