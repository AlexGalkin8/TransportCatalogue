#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "svg.h"
#include "serialization.h"

#include <optional>
#include <string>
#include <string_view>

#include "map_renderer.pb.h"

namespace transport_catalogue::request_handler
{
    class RequestHandler
    {
    public:
        RequestHandler(database::TransportCatalogue& database,
            const renderer::MapRenderer& renderer,
            const router::TransportRouter& router,
            const serialization::Serialization& serializator);

        objects::Response RequestToBase(const objects::Request& request);

        // Возвращает информацию о маршруте (запрос Bus)
        std::optional<objects::BusInfo> GetBusInfo(const std::string& bus_name) const;

        // Возвращает маршруты, проходящие через остановку
        std::optional<objects::StopInfo> GetStopInfo(const std::string& name) const;

        std::optional<router::ReportRouter> GetReportRouter(const std::string_view from,
            const std::string_view to) const;

        proto_renderer::Map RenderMap() const;

    private:
        database::TransportCatalogue& database_;
        const renderer::MapRenderer& renderer_;
        const router::TransportRouter& router_;
        const serialization::Serialization& serializator_;
    };

} // namespace transport_catalogue::request_handler
