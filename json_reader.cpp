#include "json_reader.h"
#include "json_builder.h"


using namespace std;
using namespace json;
using namespace transport_catalogue::objects;


namespace transport_catalogue
{
    namespace reader
    {
        JSONReader::JSONReader(
            database::DataBase& database,
            renderer::MapRenderer& map_renderer,
            request_handler::RequestHandler& request_handler,
            router::TransportRouter& transport_router)
            : database_(database)
            , map_renderer_(map_renderer)
            , request_handler_(request_handler)
            , transport_router_(transport_router)
        {
        }


        void JSONReader::AddRequest(istream& in)
        {
            auto load = json::Load(in).GetRoot().AsDict();
            if (load.count("base_requests"s))
                base_requests_ = load.at("base_requests"s).AsArray();
            if (load.count("stat_requests"s))
                stat_requests_ = load.at("stat_requests"s).AsArray();
            if (load.count("render_settings"s))
                map_renderer_.SetSettings(MakeRenderSettings(load.at("render_settings").AsDict()));
            if (load.count("routing_settings"s))
                transport_router_.SetSettings(MakeRouterSettings(load.at("routing_settings"s).AsDict()));

            LoadRequests();
            LoadDistances();
            LoadMapRenderer();
            LoadTransportRouter();
        }


        void JSONReader::Answer(ostream& out)
        {
            for (const auto& request : stat_requests_)
            {
                const auto& description = request.AsDict();
                const auto& type = description.at("type"s).AsString();

                if (type == "Stop"s)
                    answer_on_requests_.push_back(AnswerStop(description));
                else if (type == "Bus"s)
                    answer_on_requests_.push_back(AnswerBus(description));
                else if (type == "Map"s)
                    answer_on_requests_.push_back(AnswerMap(description));
                else if (type == "Route"s)
                    answer_on_requests_.push_back(AnswerRoute(description));
            }

            const Document report(answer_on_requests_);
            Print(report, out);
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
            map_renderer_.SetBorder(database_.GetStops());  // Определяем границы
            map_renderer_.SetTrail(database_.GetBuses());   // Отрисовываем линии маршрута
            map_renderer_.SetStation(database_.GetStops()); // Отрисовываем остановки
        }


        void JSONReader::LoadTransportRouter()
        {
            transport_router_.MakeGraph();
        }


        void JSONReader::LoadDistances()
        {
            for (const auto& request : base_requests_)
            {
                const auto& description = request.AsDict();
                if (description.at("type"s).AsString() == "Stop"s)
                    SetDistancesFromStop(description);
            }
        }


        Stop JSONReader::MakeStop(const json::Dict& description) const
        {
            Stop stop;
            stop.stop_name = description.at("name"s).AsString();
            stop.coordinates.lat = description.at("latitude"s).AsDouble();
            stop.coordinates.lng = description.at("longitude"s).AsDouble();
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


        router::Settings JSONReader::MakeRouterSettings(const json::Dict& description) const
        {
            return { description.at("bus_wait_time"s).AsInt(),
                     description.at("bus_velocity"s).AsDouble() };
        }


        Node JSONReader::AnswerStop(const json::Dict& description)
        {
            Response responce = std::move(request_handler_.RequestToBase(
                Request{ RequestType::STOP_INFO, description.at("name"s).AsString() }));

            json::Builder builder_node;
            builder_node.StartDict().Key("request_id"s).Value(description.at("id"s).AsInt());

            if (responce.responce_type == ResponseType::STOP_INFO)
            {
                StopInfo info = std::get<StopInfo>(responce.value);
                builder_node.Key("buses"s).StartArray();

                for (const string_view& bus_num : info.bus_numbers)
                    builder_node.Value(static_cast<std::string>(bus_num));
                builder_node.EndArray();
            }
            else
            {
                builder_node.Key("error_message"s).Value("not found"s);
            }

            return builder_node.EndDict().Build();
        }


        Node JSONReader::AnswerBus(const json::Dict& description)
        {
            Response responce = std::move(request_handler_.RequestToBase(
                Request{ RequestType::BUS_INFO, description.at("name"s).AsString() }));

            json::Builder builder_node;
            builder_node.StartDict()
                .Key("request_id"s).Value(description.at("id"s).AsInt());

            if (responce.responce_type == ResponseType::BUS_INFO)
            {
                BusInfo info = std::get<BusInfo>(responce.value);
                builder_node.Key("curvature"s).Value(info.curvature)
                    .Key("route_length"s).Value(static_cast<int>(info.route_length))
                    .Key("stop_count"s).Value(static_cast<int>(info.stops_on_route))
                    .Key("unique_stop_count"s).Value(static_cast<int>(info.unique_stops));
            }
            else
            {
                builder_node.Key("error_message"s).Value("not found"s);
            }

            return builder_node.EndDict().Build();
        }


        Node JSONReader::AnswerMap(const json::Dict& description)
        {
            json::Builder builder_node;
            builder_node.StartDict()
                .Key("request_id"s).Value(description.at("id"s).AsInt());

            std::ostringstream s;
            request_handler_.RenderMap().Render(s);
            builder_node.Key("map"s).Value(s.str());

            return builder_node.EndDict().Build();
        }


        json::Node JSONReader::AnswerRoute(const json::Dict& description)
        {
            using namespace graph;
            json::Builder builder_node;

            builder_node.StartDict()
                .Key("request_id"s).Value(description.at("id"s).AsInt());

            const auto& report = request_handler_.GetReportRouter(description.at("from"s).AsString(),
                description.at("to"s).AsString());

            if (report)
            {
                builder_node.Key("total_time"s).Value(report->total_minutes)
                    .Key("items"s).StartArray();

                for (const auto& info : report->information)
                {
                    builder_node.StartDict()
                        .Key("type"s).Value("Wait"s)
                        .Key("time"s).Value(info.wait.minutes)
                        .Key("stop_name"s).Value(info.wait.name)
                        .EndDict()
                        .StartDict()
                        .Key("type"s).Value("Bus"s)
                        .Key("time"s).Value(info.bus.minutes)
                        .Key("bus"s).Value(info.bus.number)
                        .Key("span_count"s).Value(info.bus.span_count)
                        .EndDict();
                }

                builder_node.EndArray();
            }
            else
            {
                builder_node.Key("error_message"s).Value("not found"s);
            }

            return builder_node.EndDict().Build();
        }


        void JSONReader::SetDistancesFromStop(const json::Dict& description)
        {
            const auto from = database_.FindStop(description.at("name"s).AsString());

            for (const auto& [stop_name, distance] : description.at("road_distances"s).AsDict())
            {
                const auto to = database_.FindStop(stop_name);
                database_.SetDistanceBetweenStops(from, to, distance.AsInt());
            }
        }

    } // namespace reader

} // namespace transport_catalogue