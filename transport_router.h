#pragma once

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <future>

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"


namespace transport_catalogue::router
{
    struct Settings
    {
        int    bus_wait_time = 1;
        double bus_velocity = 1;
    };


    struct Info
    {
        struct Wait
        {
            double      minutes = 0;
            std::string name;
        };

        struct Bus
        {
            std::string number;
            double      minutes = 0;
            int         span_count = 0;
        };

        Wait wait;
        Bus  bus;
    };


    struct ReportRouter
    {
        std::vector<Info> information;
        double            total_minutes = 0;
    };



    class TransportRouter
    {
    public:
        using Graph = graph::DirectedWeightedGraph<objects::Road>;
        using Router = graph::Router<objects::Road>;

        explicit TransportRouter(const database::TransportCatalogue& database);

        void SetSettings(const Settings& settings);

        Settings GetSettings() const;

        void MakeGraph();

        std::optional<ReportRouter> GetReportRouter(const std::string_view from,
            const std::string_view to) const;

    private:
        // Libs
        const database::TransportCatalogue& database_;
        Settings settings_;
        Graph graph_;
        std::shared_ptr<Router> router_;

        // Attributes
        std::unordered_map<std::string_view, graph::VertexId> name_id_;
        std::unordered_map<graph::VertexId, std::string_view> id_name_;
        std::unordered_set<std::string> names_;

        template <typename ItStop>
        void AddEdgesGraph(ItStop begin, ItStop end, const std::string_view name);

        void AddEdgeGraph(std::shared_ptr<objects::Stop> stop);

        graph::VertexId MakeVertexId(std::shared_ptr<objects::Stop> stop);

        inline double CalculateWeightEdge(std::shared_ptr<objects::Stop> from,
            std::shared_ptr<objects::Stop> to) const;
    };



    template<typename ItStop>
    inline void TransportRouter::AddEdgesGraph(ItStop begin, ItStop end, const std::string_view name)
    {
        using namespace graph;

        // Итерируемся по остановкам в маршруте с начала маршрута
        for (auto from_id = begin; from_id != end; ++from_id)
        {
            const VertexId from = MakeVertexId(*from_id);

            for (auto to_id = next(from_id); to_id != end; ++to_id)
            {
                const VertexId to = MakeVertexId(*to_id);

                double sum_minutes = 0;
                int span_count = 0;

                for (auto it = from_id; it != to_id; ++it)
                {
                    sum_minutes += CalculateWeightEdge(*it, *(it + 1));
                    ++span_count;
                }

                sum_minutes += settings_.bus_wait_time;

                // Добавляем ребро на граф с весом sum_minutes и количеством вершин span_count
                graph_.AddEdge({ from, to, objects::Road{std::string(name), sum_minutes, span_count} });
            }
        }
    }

} // namespace transport_catalogue::router