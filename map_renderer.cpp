#include <iterator>
#include <execution>

#include "map_renderer.h"

using namespace std;
using namespace transport_catalogue::objects;
using namespace svg;


namespace transport_catalogue
{
    namespace renderer
    {
        bool IsZero(double value)
        {
            return std::abs(value) < EPSILON;
        }

        svg::Point MakePoint(double x, double y)
        {
            return { x, y };
        }

        svg::Color MakeColor(const std::string& color)
        {
            return color;
        }

        svg::Color MakeColor(int r, int g, int b)
        {
            return svg::Rgb(r, g, b);
        }

        svg::Color MakeColor(int r, int g, int b, double a)
        {
            return svg::Rgba(r, g, b, a);
        }

        // ---------- SphereProjector ------------------    

        svg::Point SphereProjector::operator()(geo::Coordinates coords) const
        {
            return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
        }



        // ---------- MapRenderer ------------------    

        svg::Document MapRenderer::GetDocument() const
        {
            return document_;
        }

        void MapRenderer::SetSettings(const Settings& settings)
        {
            settings_ = settings;
        }


        Settings MapRenderer::GetSettings() const
        {
            return settings_;
        }


        void MapRenderer::SetBorder(const Stops& stops)
        {
            std::vector<geo::Coordinates> coord_stops;
            std::for_each(stops.begin(), stops.end(),
                [&coord_stops](const auto& stop)
                {
                    if (!stop.second->free)
                    {
                        coord_stops.push_back(stop.second->coordinates);
                    }
                });

            // передаем значения минимальных и максимальных координат, и другие значения
            sphere_projector_ = SphereProjector(coord_stops.begin(), coord_stops.end(),
                settings_.width, settings_.height, settings_.padding);
        }


        void MapRenderer::SetTrail(const Buses& buses)
        {
            for (const auto& [_, bus] : buses)
            {
                if (!bus->bus_route.route_stops.empty())
                    AddTrail(*bus); // Рисуем линию маршрута
            }
            index_color_ = 0;
            for (const auto& [_, bus] : buses)
            {
                if (!bus->bus_route.route_stops.empty())
                    AddTrailName(*bus); // ставим название маршрута в начале и конце маршрута
            }
            index_color_ = 0;
        }


        void MapRenderer::SetStation(const Stops& stops)
        {
            for (const auto& [_, stop] : stops)
            {
                if (!stop->free)
                    AddStation(*stop);
            }
            for (const auto& [_, stop] : stops)
            {
                if (!stop->free)
                    AddStationName(*stop);
            }
        }


        void MapRenderer::AddTrail(const Bus& bus)
        {
            document_.Add(CreateTrail(bus)
                .SetFillColor(NoneColor)
                .SetStrokeColor(GetColor())
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND));
        }


        void MapRenderer::AddTrailName(const Bus& bus)
        {
            if (bus.bus_route.route_stops.empty())
                return;

            const auto& point_begin = GetPoint(bus.bus_route.route_stops.front()->coordinates);
            Text text = Text().SetFillColor(GetColor())
                .SetPosition(point_begin)
                .SetOffset(settings_.bus_label_offset)
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetData(bus.number);

            Text underlayer = text;
            underlayer.SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND);

            document_.Add(underlayer);  // Добавляем подслой
            document_.Add(text);        // Добавляем поверх текст

            const auto& point_end = GetPoint(bus.bus_route.route_stops.back()->coordinates);
            if ((bus.bus_route.is_roundtrip && point_begin != point_end) || point_begin != point_end)
            {
                text.SetPosition(point_end);
                underlayer.SetPosition(point_end);
                document_.Add(underlayer);
                document_.Add(text);
            }
        }


        void MapRenderer::AddStation(const Stop& stop)
        {
            document_.Add(Circle().SetCenter(GetPoint(stop.coordinates))
                .SetRadius(settings_.stop_radius)
                .SetFillColor("white"s));
        }


        void MapRenderer::AddStationName(const Stop& stop)
        {
            Text text = Text().SetFillColor("black"s)
                .SetPosition(GetPoint(stop.coordinates))
                .SetOffset(settings_.stop_label_offset)
                .SetFontSize(settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetData(stop.stop_name);

            Text underlayer = text;
            underlayer.SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND);

            document_.Add(underlayer);
            document_.Add(text);
        }


        svg::Polyline MapRenderer::CreateTrail(const Bus& bus) const
        {
            using namespace svg;
            svg::Polyline line;
            const auto& route = bus.bus_route;

            if (route.route_stops.empty())
                return line;

            for (auto it = route.route_stops.begin(); it != route.route_stops.end(); ++it)
            {
                const auto& point = GetPoint((*it)->coordinates);
                line.AddPoint(point);
            }

            if (!route.is_roundtrip)
            {
                for (auto it = next(route.route_stops.rbegin()); it != route.route_stops.rend(); ++it)
                {
                    const auto& point = GetPoint((*it)->coordinates);
                    line.AddPoint(point);
                }
            }
            return line;
        }


        svg::Color MapRenderer::GetColor()
        {
            return settings_.color_palette.at(GetIndexColor());
        }


        svg::Point MapRenderer::GetPoint(const geo::Coordinates& coords) const
        {
            return sphere_projector_(coords);
        }


        size_t MapRenderer::GetIndexColor()
        {
            return index_color_++ % settings_.color_palette.size();
        }

    } // namespace renderer

} // namespace transport_catalogue