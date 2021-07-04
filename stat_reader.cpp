#include "stat_reader.h"
#include <iomanip>

namespace transport_catalogue
{
    namespace output
    {
        void PrintBusInfo(std::ostream& out, const BusInfo& out_info)
        {
            if (out_info.stops_on_route == 0)
            {
                out << "Bus " << out_info.bus_number << ": "
                    << "no stops" << std::endl;
            }
            else
            {
                out << "Bus " << out_info.bus_number << ": "
                    << out_info.stops_on_route << " stops on route, "
                    << out_info.unique_stops << " unique stops, "
                    << out_info.route_length << " route length, "
                    << std::setprecision(6) << out_info.curvature
                    << " curvature" << std::endl;
            }
        }

        void PrintStopInfo(std::ostream& out, const StopInfo& out_info)
        {

            if (out_info.bus_numbers.empty())
            {
                out << "Stop " << out_info.stop_name << ": "
                    << "no buses" << std::endl;
            }
            else
            {
                out << "Stop " << out_info.stop_name << ": buses";
                for (const auto bus : out_info.bus_numbers)
                    out << " " << bus;
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

                if (std::holds_alternative<BusInfo>(responce))
                    PrintBusInfo(std::cout, std::get<BusInfo>(responce));
                else if (std::holds_alternative<StopInfo>(responce))
                    PrintStopInfo(std::cout, std::get<StopInfo>(responce));
                else
                    std::cout << std::get<std::string>(responce) << std::endl;
            }
        }

        void ReadOutRequest(TransportCatalogue& transport_catalogue, std::stringstream& is)
        {
            std::string request_words;
            while (std::getline(is, request_words))
            {
                Request request(request_words);
                Responce responce = transport_catalogue.GetRequest(request);

                if (std::holds_alternative<BusInfo>(responce))
                    PrintBusInfo(std::cout, std::get<BusInfo>(responce));
                else if (std::holds_alternative<StopInfo>(responce))
                    PrintStopInfo(std::cout, std::get<StopInfo>(responce));
                else
                    std::cout << std::get<std::string>(responce) << std::endl;
            }
        }
    }
}