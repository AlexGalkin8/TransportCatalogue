#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <fstream>
#include <iostream>
#include <string_view>

using namespace transport_catalogue;
using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr)
{
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);


    database::TransportCatalogue database;
    router::TransportRouter router(database);
    renderer::MapRenderer map_renderer;
    serialization::Serialization serializator(database, map_renderer, router);
    request_handler::RequestHandler request_handler(database, map_renderer, router, serializator);
    reader::JSONReader json_reader(database, map_renderer, request_handler, router, serializator);

    if (mode == "make_base"sv)
    {
        json_reader.ReadBase();
    }
    else if (mode == "process_requests"sv)
    {
        json_reader.ReadRequests();
        json_reader.Answer();
    }
    else
    {
        PrintUsage();
        return 1;
    }

    return 0;
}
