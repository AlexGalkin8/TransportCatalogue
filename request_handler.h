#pragma once

#include "domain.h"
#include "map_renderer.h"

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
            RequestHandler(database::DataBase& base_data, const renderer::MapRenderer& renderer);

            objects::Responce RequestToBase(const objects::Request& request);

            // Возвращает информацию о маршруте (запрос Bus)
            std::optional<objects::BusInfo> GetBusInfo(const std::string& bus_name) const;

            // Возвращает маршруты, проходящие через остановку
            std::optional<objects::StopInfo> GetStopInfo(const std::string& stop_name) const;

            void AddDistanceBetweenStops(std::string& stop_name, std::map<std::string, size_t>& distances_for_stop);

            svg::Document RenderMap() const;

        private:
            // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
            database::DataBase&          database_;
            const renderer::MapRenderer& renderer_;
        };
    } // namespace request_handler

} // namespace transport_catalogue
