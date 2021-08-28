#include "transport_router.h"

using namespace std;


namespace transport_catalogue
{
    namespace router
    {
        TransportRouter::TransportRouter(const database::DataBase& base_data)
            : base_data_(base_data)
            , graph_(450)
            , router_(nullptr)
        {
        }


        void TransportRouter::SetSettings(const Settings& settings)
        {
            settings_ = settings;
        }


        Settings TransportRouter::GetSettings() const
        {
            return settings_;
        }


        void TransportRouter::MakeGraph()
        {
            // Заполняем граф остановками(добавляем вершины графа)
            for (const auto& [_, stop] : base_data_.GetStops())
                AddEdgeGraph(stop);

            // Добавляем маршруты между остановками(добавляем рёбра графа)
            for (const auto& [_, bus] : base_data_.GetBuses())
            {
                const auto& bus_route = bus->bus_route;
                const string_view number = bus->number;

                AddEdgesGraph(bus_route.route_stops.begin(), bus_route.route_stops.end(), number);
                if (!bus_route.is_roundtrip)
                {
                    AddEdgesGraph(bus_route.route_stops.rbegin(), bus_route.route_stops.rend(), number);
                }
            }

            static Router router(graph_);
            router_ = make_shared<Router>(std::move(router));
        }


        optional<ReportRouter> TransportRouter::GetReportRouter(const std::string_view from,
            const std::string_view to) const
        {
            using namespace graph;

            const VertexId start = name_id_.at(from);
            const VertexId finish = name_id_.at(to);
            const auto& info = router_->BuildRoute(start, finish);
            ReportRouter report;

            if (!info)
                return nullopt;

            report.total_minutes = info->weight.minutes;
            const auto& edges = info->edges;

            for (auto it = edges.begin(); it != edges.end(); ++it)
            {
                Edge<objects::Road> edge = graph_.GetEdge(*it);

                Info::Wait info_wait;
                info_wait.name = string(id_name_.at(edge.from));
                info_wait.minutes = settings_.bus_wait_time;

                Info::Bus info_bus;
                info_bus.number = edge.weight.bus_number;
                info_bus.span_count = edge.weight.span_count;
                info_bus.minutes = edge.weight.minutes - settings_.bus_wait_time;
                report.information.push_back(Info{ info_wait, info_bus });
            }

            return report;
        }


        void TransportRouter::AddEdgeGraph(std::shared_ptr<objects::Stop> stop)
        {
            using namespace graph;

            VertexId begin, end;
            begin = end = MakeVertexId(stop);
            graph_.AddEdge({ begin, end, objects::Road{} });
        }


        graph::VertexId TransportRouter::MakeVertexId(shared_ptr<objects::Stop> stop)
        {
            using namespace graph;

            if (name_id_.count(stop->stop_name) != 0)
                return name_id_.at(stop->stop_name);

            const auto& [it, _] = names_.emplace(stop->stop_name);
            const string_view name = *it;
            const VertexId id = name_id_[name] = name_id_.size();
            id_name_[id] = name;

            return id;
        }


        double TransportRouter::CalculateWeightEdge(std::shared_ptr<objects::Stop> from,
            std::shared_ptr<objects::Stop> to) const
        {
            return 60.0 * base_data_.GetDistanceBetweenStops(from, to) / (1000.0 * settings_.bus_velocity);
        }

    } // namespace router

} // namespace transport_catalogue