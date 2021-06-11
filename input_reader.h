#pragma once

#include "transport_catalogue.h"
#include <string_view>
#include <algorithm>
#include <iostream>

void ReadInputRequest(TransportCatalogue& transport_catalogue, std::istream& is)
{
    size_t num_request = 0;
    std::string request_words;
    std::getline(is, request_words);
    num_request = std::stoi(request_words);
    for (size_t i = 0; i < num_request; i++)
    {
        std::getline(is, request_words);
        Request request(request_words);
        transport_catalogue.GetRequest(request);
    }
}


