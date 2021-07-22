#include "json_reader.h"

using namespace std;
using namespace json;
using namespace transport_catalogue::objects;



namespace transport_catalogue
{
    namespace reader
    {
        JSONReader::JSONReader(database::DataBase& database,
                               renderer::MapRenderer& map_renderer,
                               request_handler::RequestHandler& request_handler)
            : database_(database)
            , map_renderer_(map_renderer)
            , request_handler_(request_handler)
        {
        }


        void JSONReader::AddRequest(istream& in)
        {
            auto load = json::Load(in).GetRoot().AsDict();
            if (load.count("base_requests"s))
            {
                base_requests_ = load.at("base_requests"s).AsArray();
            }
            if (load.count("stat_requests"s))
            {
                stat_requests_ = load.at("stat_requests"s).AsArray();
            }
            if (load.count("render_settings"s))
            {
                map_renderer_.SetSettings(MakeRenderSettings(load.at("render_settings").AsDict()));
            }

            LoadRequests();
            LoadMapRenderer();
        }


        void JSONReader::Answer(ostream& out)
        {
            out << "[" << endl;

            bool b = true;
            for (const auto& request : stat_requests_)
            {
                const auto& description = request.AsDict();
                const auto& type = description.at("type"s).AsString();
                if (b)
                {
                    if (type == "Stop"s)
                    {
                        out << AnswerStop(description);
                        b = false;
                    }
                    else if (type == "Bus"s)
                    {
                        out << AnswerBus(description);
                        b = false;
                    }
                    else if (type == "Map"s)
                    {
                        b = false;
                        out << AnswerMap(description);
                    }

                }
                else
                {
                    if (type == "Stop"s)
                        out << ","s << endl << AnswerStop(description);
                    else if (type == "Bus"s)
                        out << ","s << endl << AnswerBus(description);
                    else if (type == "Map"s)
                        out << ","s << endl << AnswerMap(description);
                }
            }
            out << endl << "]";
        }


        void JSONReader::LoadRequests()
        {
            for (const auto& request : base_requests_)
            {
                const auto& description = request.AsDict();
                if (description.at("type"s).AsString() == "Stop"s)
                    database_.AddStop(std::move(MakeStop(description)));
                else if (description.at("type"s).AsString() == "Bus"s)
                    database_.AddBus(std::move(MakeBus(description)));
            }
        }


        void JSONReader::LoadMapRenderer()
        {
            map_renderer_.SetBorder(database_.GetStops()); // Определяем границы
            map_renderer_.SetTrail(database_.GetBuses()); // Отрисовываем линии маршрута
            map_renderer_.SetStation(database_.GetStops()); // Отрисовываем остановки
        }


        Stop JSONReader::MakeStop(const json::Dict& description) const
        {
            Stop stop;

            stop.stop_name = description.at("name"s).AsString();
            stop.coordinates.lat = description.at("latitude"s).AsDouble();
            stop.coordinates.lng = description.at("longitude"s).AsDouble();

            if (description.count("road_distances"s))
            {
                std::map<std::string, size_t> distances;
                for (const auto& [stop_name, distance] : description.at("road_distances"s).AsDict())
                    distances.emplace(stop_name, distance.AsInt());
                request_handler_.AddDistanceBetweenStops(stop.stop_name, distances);
            }

            return stop;
        }


        Bus JSONReader::MakeBus(const json::Dict& description) const
        {
            Bus bus;
            Route route;

            bus.number = description.at("name"s).AsString();
            route.is_roundtrip = description.at("is_roundtrip"s).AsBool();

            const auto& stops = description.at("stops"s).AsArray();
            for (const auto& s : stops)
            {
                Stop stop;
                stop.stop_name = s.AsString();
                route.route_stops.push_back(std::make_shared<Stop>(stop));
            }
            bus.bus_route = std::move(route);

            return bus;
        }


        renderer::Settings JSONReader::MakeRenderSettings(const json::Dict& description) const
        {
            
            auto make_point = [](const json::Node& node)
            {
                using namespace renderer;
                const auto& array = node.AsArray();
                return MakePoint(array.front().AsDouble(),
                    array.back().AsDouble());
            };

            auto make_color = [](const json::Node& node)
            {
                using namespace renderer;
                if (node.IsArray())
                {
                    const auto& array = node.AsArray();
                    if (array.size() == 3)
                    {
                        return MakeColor(array.at(0).AsInt(),
                            array.at(1).AsInt(), array.at(2).AsInt());
                    }
                    else if (array.size() == 4)
                    {
                        return MakeColor(array.at(0).AsInt(),
                            array.at(1).AsInt(), array.at(2).AsInt(),
                            array.at(3).AsDouble());
                    }
                }
                return MakeColor(node.AsString());
            };

            renderer::Settings settings;
            settings.width = description.at("width"s).AsDouble();
            settings.height = description.at("height"s).AsDouble();
            settings.padding = description.at("padding"s).AsDouble();
            settings.line_width = description.at("line_width"s).AsDouble();
            settings.stop_radius = description.at("stop_radius"s).AsDouble();
            settings.bus_label_font_size = description.at("bus_label_font_size"s).AsInt();
            settings.bus_label_offset = make_point(description.at("bus_label_offset"s));
            settings.stop_label_font_size = description.at("stop_label_font_size"s).AsInt();
            settings.stop_label_offset = make_point(description.at("stop_label_offset"s));
            settings.underlayer_color = make_color(description.at("underlayer_color"s));
            settings.underlayer_width = description.at("underlayer_width"s).AsDouble();
            const auto& array_color = description.at("color_palette"s).AsArray();
            settings.color_palette.reserve(array_color.size());

            for (const auto& color : array_color)
            {
                settings.color_palette.push_back(make_color(color));
            }

            return settings;
        }


        std::string JSONReader::AnswerStop(const json::Dict& description)
        {
            Responce responce = std::move(request_handler_.RequestToBase(
                Request{ RequestType::STOP_INFO, description.at("name"s).AsString() }));

            std::stringstream in("");

            if (responce.responce_type == ResponceType::STOP_INFO)
            {
                StopInfo info = std::get<StopInfo>(responce.value);

                in << "    {" << std::endl
                    << "        " << "\"buses\": [" << std::endl;
                bool b = true;
                for (const string_view& bus_num : info.bus_numbers)
                {
                    if (b)
                    {
                        in << "            \"" << bus_num << "\"";
                        b = false;
                    }
                    else
                        in << "," << std::endl << "            \"" << bus_num << "\"";
                }
                in << std::endl << "        ]," << std::endl << "        \"request_id\": " << description.at("id"s).AsInt()
                    << std::endl << "    }";
            }
            else
            {
                in << "    {" << std::endl
                    << "        \"error_message\": " << "\"not found\"" << "," << std::endl
                    << "        \"request_id\": " << description.at("id"s).AsInt() << std::endl << "    }";
            }

            return in.str();
        }


        std::string JSONReader::AnswerBus(const json::Dict& description)
        {
            Responce responce = std::move(request_handler_.RequestToBase(
                Request{ RequestType::BUS_INFO, description.at("name"s).AsString() }));

            std::stringstream in("");
            int req_id = description.at("id"s).AsInt();

            if (responce.responce_type == ResponceType::BUS_INFO)
            {
                BusInfo info = std::get<BusInfo>(responce.value);
                in << "    {"s << std::endl
                    << "        "s << "\"curvature\": "s << info.curvature << ","s << endl
                    << "        "s << "\"request_id\": "s << req_id << ","s << endl
                    << "        "s << "\"route_length\": "s << info.route_length << ","s << endl
                    << "        "s << "\"stop_count\": "s << info.stops_on_route << ","s << endl
                    << "        "s << "\"unique_stop_count\": "s << info.unique_stops << endl
                    << "    }";
            }
            else
            {
                in << "    {" << std::endl
                    << "        \"error_message\": " << "\"not found\"" << "," << std::endl
                    << "        \"request_id\": " << description.at("id"s).AsInt() << std::endl << "    }";
            }

            return in.str();
        }


        std::string JSONReader::AnswerMap(const json::Dict& description)
        {

            std::stringstream in("");
            int req_id = description.at("id"s).AsInt();
            in << "    {"s << std::endl
               << "        "s << "\"map\": "s;
            //request_handler_.RenderMap().Render(in);
            std::ostringstream s;
            request_handler_.RenderMap().Render(s);
            in << json::Node(std::move(s.str()));

            in << ","s << endl
                << "        "s << "\"request_id\": "s << req_id << endl
                << "    }";

            return in.str();
        }

    } // namespace reader

} // namespace transport_catalogue