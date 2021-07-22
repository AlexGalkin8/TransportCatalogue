#pragma once

#include <iostream>
#include <sstream>
#include <string_view>
#include <variant>

#include "map_renderer.h"
#include "request_handler.h"
#include "domain.h"

namespace transport_catalogue
{
    namespace reader
    {
        // Класс, для работы с информацией в формате строк
        class StringReader
        {
        public:
            StringReader() = delete;

            StringReader(database::DataBase& database,
                            renderer::MapRenderer& map_renderer,
                            request_handler::RequestHandler& request_handler);

            void AddRequest(std::istream& is, std::ostream& out);

        private:
            void PrintResponce(const objects::Responce& responce, std::ostream& out) const;

            void PrintBusInfo(std::ostream& out, const objects::BusInfo& out_info) const;

            void PrintStopInfo(std::ostream& out, const objects::StopInfo& out_info) const;

            objects::Responce ReadRequest(std::string& request_words);

        private:
            database::DataBase&              database_;         // Ссылка на базу данных Транспортого справочника
            renderer::MapRenderer&           map_renderer_;     // Ссылка на класс, визуализирующий информацию Транспортного справочника
            request_handler::RequestHandler& request_handler_;  // Ссылка на класс, представляющий собой базовый функционал для запросов в базу
        };



        class StrRequestParser
        {
        public:
            StrRequestParser() = default;
            StrRequestParser(std::string_view request_str);

            void                     ParseRequest(std::string_view request_str);

            const objects::Request& GetRequest() const;

            const std::map<std::string, size_t>& GetDistances() const;

            static std::string_view  ReadWord(std::string_view& str, const char end_char = ' ');

        private:
            objects::RequestType     ParseRequestType(std::string_view request_str) const;

            void                     ParseAddRouteRequest(std::string_view request_str);

            void                     ParseAddStopRequest(std::string_view request_str);

        private:
            objects::Request              request_;
            std::map<std::string, size_t> distances_;
        };

    } // namespace reader

} // namespace transport_catalogue