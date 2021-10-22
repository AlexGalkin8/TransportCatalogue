#pragma once

#include "domain.h"

#include <memory>
#include <string>


namespace transport_catalogue::database
{
    class TransportCatalogue
    {
    public:
        const std::shared_ptr<objects::Bus> FindBus(const std::string& bus_name) const;

        const std::shared_ptr<objects::Stop> FindStop(const std::string& name) const;

        const objects::Buses& GetBuses() const;

        const objects::Stops& GetStops() const;

        void AddStop(const objects::Stop& stop);

        void AddBus(const objects::Bus& bus);

        void SetDistanceBetweenStops(std::shared_ptr<objects::Stop> from, std::shared_ptr<objects::Stop> to, size_t distance);

        size_t GetDistanceBetweenStops(std::shared_ptr<objects::Stop> from, std::shared_ptr<objects::Stop> to) const;

        const objects::StopsDistance& GetStopsDistance() const;

    private:
        objects::Buses buses_;
        objects::Stops stops_;

        objects::StopsDistance length_between_stops_;
    };
} // namespace transport_catalogue::database