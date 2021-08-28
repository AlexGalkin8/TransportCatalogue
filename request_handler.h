#pragma once

//#include "domain.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>


namespace transport_catalogue
{
    namespace request_handler
    {
        class RequestHandler
        {
        public:
            RequestHandler(database::DataBase& base_data,
                const renderer::MapRenderer& renderer,
                const router::TransportRouter& router);

            objects::Response RequestToBase(const objects::Request& request);

            // Возвращает информацию о маршруте (запрос Bus)
            std::optional<objects::BusInfo> GetBusInfo(const std::string& bus_name) const;

            // Возвращает маршруты, проходящие через остановку
            std::optional<objects::StopInfo> GetStopInfo(const std::string& name) const;

            std::optional<router::ReportRouter> GetReportRouter(const std::string_view from,
                const std::string_view to) const;

            svg::Document RenderMap() const;

        private:
            // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
            database::DataBase& database_;
            const renderer::MapRenderer& renderer_;
            const router::TransportRouter& router_;
        };

    } // namespace request_handler

} // namespace transport_catalogue
