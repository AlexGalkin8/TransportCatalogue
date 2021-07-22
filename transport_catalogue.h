#pragma once

#include "domain.h"
#include "request_handler.h"
#include "input_reader.h"
#include "json_reader.h"

namespace transport_catalogue
{
    enum class RequestFormat { JSON = 0, STR = 1 };

    class TransportCatalogue
    {
    public:
        TransportCatalogue(RequestFormat request_format = RequestFormat::STR);
        void AddRequest(std::istream& is = std::cin, std::ostream& out = std::cout);

    private:
        RequestFormat request_format_;
        database::DataBase database_;
    };
} // namespace transport_catalogue