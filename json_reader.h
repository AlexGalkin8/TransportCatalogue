#pragma once

#include "json.h"
#include "domain.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
#include "serialization.h"
#include "transport_catalogue.h"

#include <filesystem>
#include <iostream>


namespace transport_catalogue::reader
{
    class JSONReader
    {
        using Path = std::filesystem::path;
    public:
        JSONReader() = delete;

        JSONReader(
            database::TransportCatalogue& database,
            renderer::MapRenderer& map_renderer,
            request_handler::RequestHandler& request_handler,
            router::TransportRouter& transport_router,
            serialization::Serialization& serializator);


        void ReadBase(std::istream& in = std::cin);

        void ReadRequests(std::istream& in = std::cin);

        void Answer(std::ostream& out = std::cout);

    private:
        void LoadTransportCatalogue();

        void LoadMapRenderer();

        void LoadTransportRouter();

        void LoadStops();

        void LoadDistances();

        void LoadBuses();


        objects::Stop MakeStop(const json::Dict& description) const;

        objects::Bus MakeBus(const json::Dict& description) const;


        renderer::Settings MakeRenderSettings(const json::Dict& description) const;

        router::Settings MakeRouterSettings(const json::Dict& description) const;

        Path MakeSerializationSetting(const json::Dict& description) const;


        json::Node AnswerStop(const json::Dict& description);

        json::Node AnswerBus(const json::Dict& description);

        json::Node AnswerMap(const json::Dict& description);

        json::Node AnswerRoute(const json::Dict& description);


        void SetDistancesFromStop(const json::Dict& description);

    private:
        database::TransportCatalogue& database_;
        renderer::MapRenderer& map_renderer_;
        request_handler::RequestHandler& request_handler_;
        router::TransportRouter& transport_router_;
        serialization::Serialization& serializator_;

        json::Array base_requests_;
        json::Array stat_requests_;
        json::Array answer_on_requests_;
    };

} // namespace transport_catalogue