#include "input_reader.h"

#include <iomanip>

using namespace transport_catalogue::objects;


namespace transport_catalogue
{
    namespace reader
    {
        /****************************************************
        ****************   StringReader   *****************
        *****************************************************/

        StringReader::StringReader(database::DataBase& database,
                                       renderer::MapRenderer& map_renderer,
                                       request_handler::RequestHandler& request_handler)
            : database_(database)
            , map_renderer_(map_renderer)
            , request_handler_(request_handler)
        {
        }


        void StringReader::AddRequest(std::istream& is, std::ostream& out)
        {
            size_t num_request = 0;
            std::string request_words;

            // Читаем количество запросов
            std::getline(is, request_words);
            num_request = std::stoi(request_words);

            for (size_t i = 0; i < num_request; i++)
            {
                std::getline(is, request_words);
                Response responce = ReadRequest(request_words);

                if (responce.responce_type != ResponseType::EMPTY)
                    PrintResponce(responce, out);
            }
        }


        void StringReader::PrintResponce(const Response& responce, std::ostream& out) const
        {
            if (responce.responce_type == ResponseType::BUS_INFO)
                PrintBusInfo(out, std::get<BusInfo>(responce.value));
            else if (responce.responce_type == ResponseType::STOP_INFO)
                PrintStopInfo(out, std::get<StopInfo>(responce.value));
            else
                std::cout << std::get<std::string>(responce.value) << std::endl;
        }


        void StringReader::PrintBusInfo(std::ostream& out, const BusInfo& out_info) const
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


        void StringReader::PrintStopInfo(std::ostream& out, const StopInfo& out_info) const
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


        Response StringReader::ReadRequest(std::string& request_words)
        {
            StringRequestParser request_parser(request_words);
            Request request = request_parser.GetRequest();
            Response responce = request_handler_.RequestToBase(request);

            auto dis = request_parser.GetDistances();

            if (request.request_type == RequestType::ADD_STOP && !dis.empty())
            {
                request_handler_.AddDistanceBetweenStops(std::get<Stop>(request.value).stop_name, dis);
            }

            return responce;
        }



        /****************************************************
        ***************   StringRequestParser   ****************
        *****************************************************/


        StringRequestParser::StringRequestParser(std::string_view request_str)
        {
            ParseRequest(request_str);
        }


        const Request& StringRequestParser::GetRequest() const
        {
            return request_;
        }


        const std::map<std::string, size_t>& StringRequestParser::GetDistances() const
        {
            return distances_;
        }


        void StringRequestParser::ParseRequest(std::string_view request_str)
        {
            if (request_str.empty())
                throw std::invalid_argument("ParseRequests: Empty request!");

            // Определяем какой тип запроса
            request_.request_type = ParseRequestType(request_str);

            // Парсим запрос, в зависимости от его типа
            if (request_.request_type == RequestType::ADD_BUS)
            {
                ParseAddRouteRequest(request_str);
            }
            else if (request_.request_type == RequestType::ADD_STOP)
            {
                ParseAddStopRequest(request_str);
            }
            else if (request_.request_type == RequestType::BUS_INFO)
            {
                request_str.remove_prefix(4);
                request_.value = static_cast<std::string>(ReadWord(request_str, '\n'));
            }
            else if (request_.request_type == RequestType::STOP_INFO)
            {
                request_str.remove_prefix(5);
                request_.value = static_cast<std::string>(ReadWord(request_str, '\n'));
            }
        }


        std::string_view StringRequestParser::ReadWord(std::string_view& str, const char end_char)
        {
            if (str.size() == 0)
                return str;

            std::string_view out = str;
            bool need_reduction = false;

            for (size_t i = 0; i < str.size(); i++)
            {
                if (out.at(i) == end_char)
                {
                    out.remove_suffix(out.size() - i);
                    need_reduction = true;
                    break;
                }
            }

            if (end_char == ' ' && need_reduction)
            {
                str.remove_prefix(out.size() + 1);
            }
            else if ((end_char == '>' || end_char == '-') && need_reduction)
            {
                out.remove_suffix(1);
                str.remove_prefix(out.size() + 3);
            }
            else if (end_char == 'm')
            {
                str.remove_prefix(out.size() + 5);
            }
            else if (need_reduction)
            {
                if (out.size() + 2 > str.size())
                    str.remove_prefix(out.size() + 1);
                else
                    str.remove_prefix(out.size() + 2);
            }
            else
            {
                str.remove_prefix(out.size());
            }

            return out;
        }


        RequestType StringRequestParser::ParseRequestType(std::string_view request_str) const
        {
            RequestType request_type = RequestType::UNKNOWN;

            std::string_view word = ReadWord(request_str);

            if (word == "Bus")
            {
                // Проверяем последний символ - есть ':' или нет
                if (request_str.find(':') != std::string_view::npos)
                    request_type = RequestType::ADD_BUS;
                else
                    request_type = RequestType::BUS_INFO;
            }
            else if (word == "Stop")
            {
                if (request_str.find(':') != std::string_view::npos)
                    request_type = RequestType::ADD_STOP;
                else
                    request_type = RequestType::STOP_INFO;
            }

            return request_type;
        }


        void StringRequestParser::ParseAddRouteRequest(std::string_view request_str)
        {
            request_str.remove_prefix(4); // Пропускаем символы "Bus " в запросе

            Bus bus;
            Route route;

            // Читаем номер автобуса
            std::string_view word = ReadWord(request_str, ':');
            bus.number = static_cast<std::string>(word);

            bool is_roundtrip = request_str.find('>') != std::string_view::npos;
            char delimiter = (is_roundtrip) ? '>' : '-';

            // Заполнение параметров маршрута
            route.is_roundtrip = !is_roundtrip;
            while (request_str.size() != 0)
            {
                Stop stop;
                word = ReadWord(request_str, delimiter);
                stop.stop_name = std::string{ word };
                route.route_stops.push_back(std::make_shared<Stop>(stop));
            }

            bus.bus_route = route;
            request_.value = bus;
        }


        void StringRequestParser::ParseAddStopRequest(std::string_view request_str)
        {
            request_str.remove_prefix(5); // Пропускаем символы "Stop " в запросе

            Stop stop;

            // Читаем и записываем название остановки
            std::string_view word = ReadWord(request_str, ':');
            stop.stop_name = static_cast<std::string>(word);

            word = ReadWord(request_str, ',');
            bool is_distance_list = request_str.find(',') != std::string_view::npos;

            if (is_distance_list)
            {
                stop.coordinates.lat = std::stod(std::string{ word });
                word = ReadWord(request_str, ',');
                stop.coordinates.lng = std::stod(std::string{ word });

                std::vector<std::string_view> info_for_distance;
                while (request_str.size() != 0)
                {
                    if (request_str.find(',') != std::string_view::npos)
                    {
                        word = ReadWord(request_str, ',');
                        info_for_distance.push_back(word);
                    }
                    else
                    {
                        word = ReadWord(request_str, '\n');
                        info_for_distance.push_back(word);
                    }
                }

                for (std::string_view& info : info_for_distance)
                {
                    size_t distance = std::stoi(std::string{ ReadWord(info, 'm') });
                    distances_[std::string{ info }] = distance;
                }
            }
            else
            {
                stop.coordinates.lat = std::stod(std::string{ word });
                stop.coordinates.lng = std::stod(std::string{ request_str });
            }

            request_.value = stop;
        }

    } // namespace reader

} // namespace transport_catalogue