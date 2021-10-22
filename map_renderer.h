#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include "geo.h"
#include "svg.h"

#include <cstdint>
#include <optional>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace transport_catalogue::renderer
{
    struct Settings
    {
        double                            width = 0;
        double                            height = 0;
        double                            padding = 0;
        double                            line_width = 0;
        double                            stop_radius = 0;
        uint64_t                          bus_label_font_size = 0;
        svg::Point                        bus_label_offset;
        uint64_t                          stop_label_font_size = 0;
        svg::Point                        stop_label_offset;
        svg::Color                        underlayer_color;
        double                            underlayer_width = 0;
        std::vector<svg::Color>           color_palette;
    };


    inline const double EPSILON = 1e-6;

    bool IsZero(double value);

    svg::Point MakePoint(double x, double y);

    svg::Color MakeColor(const std::string& color);

    svg::Color MakeColor(int r, int g, int b);

    svg::Color MakeColor(int r, int g, int b, double a);


    class SphereProjector
    {
    public:
        SphereProjector() = default;

        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
            double max_height, double padding);

        svg::Point operator()(geo::Coordinates coords) const;

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };



    class MapRenderer final
    {
    public:
        MapRenderer() = default;

        svg::Document GetDocument() const;

        void SetSettings(const Settings& settings);

        Settings GetSettings() const;

        // Определяет границы карты, по переданным остановкам
        void SetBorder(const objects::Stops& stops);

        // Определяет границы карты, по переданным остановкам
        void SetTrail(const objects::Buses& buses);

        void SetStation(const objects::Stops& stops);

    private:
        void AddTrail(const objects::Bus& bus);

        void AddTrailName(const objects::Bus& bus);

        void AddStation(const objects::Stop& stop);

        void AddStationName(const objects::Stop& stop);

        svg::Polyline CreateTrail(const objects::Bus& bus) const;

        svg::Color GetColor();

        svg::Point GetPoint(const geo::Coordinates& coords) const;

        size_t GetIndexColor();

    private:
        SphereProjector sphere_projector_;
        Settings        settings_;
        size_t          index_color_ = 0;
        svg::Document   document_;
    };



    template <typename PointInputIt>
    SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding)
        : padding_(padding)
    {
        if (points_begin == points_end)
            return;

        const auto [left_it, right_it]
            = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
                { return lhs.lng < rhs.lng; });

        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it]
            = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
                { return lhs.lat < rhs.lat; });

        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_))
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat))
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);

        if (width_zoom && height_zoom)
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        else if (width_zoom)
            zoom_coeff_ = *width_zoom;
        else if (height_zoom)
            zoom_coeff_ = *height_zoom;
    }

} // namespace transport_catalogue::renderer