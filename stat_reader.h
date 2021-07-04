#pragma once

#include "transport_catalogue.h"
#include <iostream>
#include <sstream>

namespace transport_catalogue
{
    namespace output
    {
        void PrintBusInfo(std::ostream& out, const BusInfo& out_info);

        void PrintStopInfo(std::ostream& out, const StopInfo& out_info);


        void ReadOutRequest(TransportCatalogue& transport_catalogue, std::istream& is);

        void ReadOutRequest(TransportCatalogue& transport_catalogue, std::stringstream& is);
    }
}