#pragma once

#include <filesystem>
#include <iostream>

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "domain.h"

namespace transport_catalogue
{
    namespace reader
    {
        class JSONreader
        {
        public:
            JSONreader() = delete;

            JSONreader(database::DataBase& database,
                       renderer::MapRenderer& map_renderer,
                       request_handler::RequestHandler& request_handle);

            void AddRequest(std::istream& in = std::cin);

            void Answer(std::ostream& out = std::cout);

        private:
            void LoadRequests();

            void LoadMapRenderer();

            objects::Stop MakeStop(const json::Dict& description) const;

            objects::Bus MakeBus(const json::Dict& description) const;

            renderer::Settings MakeRenderSettings(const json::Dict& description) const;

            // Составление ответа на запрос информации об остановке в формате JSON
            std::string AnswerStop(const json::Dict& description);

            // Составление ответа на запрос информации о маршруте в формате JSON
            std::string AnswerBus(const json::Dict& description);

            std::string AnswerMap(const json::Dict& description);

        private:
            database::DataBase&              database_;         // Ссылка на базу данных Транспортого справочника
            renderer::MapRenderer&           map_renderer_;     // Ссылка на класс, визуализирующий информацию Транспортного справочника
            request_handler::RequestHandler& request_handler_;  // Ссылка на класс, представляющий собой базовый функционал для запросов в базу

            json::Array base_requests_;       // Базовый запрос в базу "base_requests"
            json::Array stat_requests_;       // Запрос в базу на получение информации "stat_requests"
        };

    } // namespace reader

} // namespace transport_catalogue