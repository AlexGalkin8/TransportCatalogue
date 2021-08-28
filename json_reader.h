#pragma once

#include <filesystem>
#include <iostream>

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
//#include "domain.h"

namespace transport_catalogue
{
    namespace reader
    {
        class JSONReader
        {
        public:
            JSONReader() = delete;

            JSONReader(
                database::DataBase& database,
                renderer::MapRenderer& map_renderer,
                request_handler::RequestHandler& request_handler,
                router::TransportRouter& transport_router);

            void AddRequest(std::istream& in = std::cin);

            void Answer(std::ostream& out = std::cout);

        private:
            void LoadRequests();

            void LoadMapRenderer();

            void LoadTransportRouter();

            void LoadDistances();


            objects::Stop MakeStop(const json::Dict& description) const;

            objects::Bus MakeBus(const json::Dict& description) const;


            renderer::Settings MakeRenderSettings(const json::Dict& description) const;

            router::Settings MakeRouterSettings(const json::Dict& description) const;


            json::Node AnswerStop(const json::Dict& description);

            json::Node AnswerBus(const json::Dict& description);

            json::Node AnswerMap(const json::Dict& description);

            json::Node AnswerRoute(const json::Dict& description);

        private:
            database::DataBase&              database_;
            renderer::MapRenderer&           map_renderer_;
            request_handler::RequestHandler& request_handler_;
            router::TransportRouter&         transport_router_;

            json::Array base_requests_;
            json::Array stat_requests_;
            json::Array answer_on_requests_;

            void SetDistancesFromStop(const json::Dict& description);
        };

    } // namespace reader

} // namespace transport_catalogue