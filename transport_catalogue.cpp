#include "transport_catalogue.h"

#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <utility>
#include <functional>

using namespace geo;
using namespace transport_catalogue::objects;

namespace transport_catalogue
{
    TransportCatalogue::TransportCatalogue(RequestFormat request_format)
        : request_format_(request_format)
    {
    }


    void TransportCatalogue::AddRequest(std::istream& is, std::ostream& out)
    {
        renderer::MapRenderer           map_renderer;
        request_handler::RequestHandler request_handler(database_, map_renderer);

        if (request_format_ == RequestFormat::JSON)
        {
            reader::JSONreader json_reader(database_, map_renderer, request_handler);
            json_reader.AddRequest(is);

            setlocale(LC_ALL, "rus");
            json_reader.Answer(out);
        }
        if (request_format_ == RequestFormat::STR)
        {
            reader::StringReader string_reader(database_, map_renderer, request_handler);
            string_reader.AddRequest(is, out);
        }
    }
} // namespace transport_catalogue