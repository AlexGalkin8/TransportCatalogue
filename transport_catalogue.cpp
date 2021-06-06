#include "transport_catalogue.h"
#include <sstream>
#include <exception>

void ParseRequests(std::string request)
{
    if (request.size() == 0)
        throw std::invalid_argument("ParseRequests: Empty request!");

    std::istringstream query_request(request);

    std::string command;
    query_request >> command;

    if (command == "Stop")
    {

    }
    else if (command == "Bus")
    {

    }
}