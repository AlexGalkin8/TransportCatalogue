#pragma once

#include "transport_catalogue.h"
#include <iostream>
#include <sstream>

namespace transport_catalogue
{
    namespace output
    {
        void PrintRouteInfo(std::ostream& out, const Responce& responce);

        void PrintStopInfo(std::ostream& out, const Responce& responce);


        void ReadOutRequest(TransportCatalogue& transport_catalogue, std::istream& is);

        void ReadOutRequest(TransportCatalogue& transport_catalogue, std::stringstream& is);
    }
}