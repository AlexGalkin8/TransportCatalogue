#include "serialization.h"

#include <fstream>
#include <sstream>

using namespace std;


namespace transport_catalogue::serialization
{
    Serialization::Serialization(
        database::TransportCatalogue& database,
        renderer::MapRenderer& map_renderer,
        router::TransportRouter& transport_router)
        : database_(database)
        , map_renderer_(map_renderer)
        , transport_router_(transport_router)
    {
    }

    void Serialization::SetSetting(const Path& path_to_base)
    {
        path_to_base_ = path_to_base;
    }

    void Serialization::CreateBase()
    {
        ofstream out_file(path_to_base_, ios::binary);

        SaveStops();
        SaveStopsDistance();
        SaveBuses();
        SaveMap();
        SaveParameters();
        base_.SerializeToOstream(&out_file);
    }

    void Serialization::AccessBase()
    {
        ifstream in_file(path_to_base_, ios::binary);
        base_.ParseFromIstream(&in_file);
        LoadStops();
        LoadStopsDistance();
        LoadBuses();
        LoadParameters();
    }

    proto_tc::Stop Serialization::SaveStop(const objects::Stop& stop) const
    {
        proto_tc::Stop result;
        result.set_name(stop.stop_name);
        result.mutable_coords()->set_lat(stop.coordinates.lat);
        result.mutable_coords()->set_lng(stop.coordinates.lng);
        result.set_free(stop.free);
        return result;
    }

    proto_tc::StopsDistance Serialization::SaveStopsDistance(
        shared_ptr<objects::Stop> from,
        shared_ptr<objects::Stop> to,
        uint64_t distance) const
    {
        proto_tc::StopsDistance result;
        result.set_from(from->stop_name);
        result.set_to(to->stop_name);
        result.set_distance(distance);
        return result;
    }

    proto_tc::Bus Serialization::SaveBus(const objects::Bus& bus) const
    {
        proto_tc::Bus result;
        proto_tc::Route route;

        result.set_number(bus.number);

        route.set_is_roundtrip(bus.bus_route.is_roundtrip);

        for (const auto& stop : bus.bus_route.route_stops)
        {
            route.add_stop_names(stop->stop_name);
        }

        *result.mutable_bus_route() = route;
        return result;
    }

    void Serialization::SaveStops()
    {
        for (const auto& [_, stop] : database_.GetStops())
        {
            *base_.add_stops() = move(SaveStop(*stop));
        }
    }

    void Serialization::SaveStopsDistance()
    {
        for (const auto& [from_to, distance] : database_.GetStopsDistance())
        {
            *base_.add_length_between_stops() = move(SaveStopsDistance(from_to.first, from_to.second, distance));
        }
    }

    void Serialization::SaveBuses()
    {
        for (const auto& [_, bus] : database_.GetBuses())
        {
            *base_.add_buses() = move(SaveBus(*bus));
        }
    }

    void Serialization::LoadStop(const proto_tc::Stop& stop)
    {
        objects::Stop target;

        target.stop_name = stop.name();
        target.coordinates.lat = stop.coords().lat();
        target.coordinates.lng = stop.coords().lng();
        target.free = stop.free();

        database_.AddStop(target);
    }


    void Serialization::LoadStopsDistance(const proto_tc::StopsDistance& from_to_distance)
    {
        const auto& from = database_.FindStop(from_to_distance.from());
        const auto& to = database_.FindStop(from_to_distance.to());
        database_.SetDistanceBetweenStops(from, to, from_to_distance.distance());
    }


    void Serialization::LoadBus(const proto_tc::Bus& bus)
    {
        objects::Bus target;

        target.number = bus.number();
        target.bus_route.is_roundtrip = bus.bus_route().is_roundtrip();
        for (int i = 0; i < bus.bus_route().stop_names_size(); ++i)
        {
            target.bus_route.route_stops.push_back(database_.FindStop(bus.bus_route().stop_names(i)));
        }

        database_.AddBus(target);
    }

    void Serialization::LoadStops()
    {
        for (int i = 0; i < base_.stops_size(); ++i)
        {
            LoadStop(base_.stops(i));
        }
    }

    void Serialization::LoadStopsDistance()
    {
        for (int i = 0; i < base_.length_between_stops_size(); ++i)
        {
            LoadStopsDistance(base_.length_between_stops(i));
        }
    }

    void Serialization::LoadBuses()
    {
        for (int i = 0; i < base_.buses_size(); ++i)
        {
            LoadBus(base_.buses(i));
        }
    }

   
    void Serialization::SaveMap() const
    {
        std::ostringstream out;
        map_renderer_.GetDocument().Render(out);
        base_.mutable_map()->set_description(out.str());
    }

    proto_renderer::Map Serialization::LoadMap() const
    {
        return base_.map();
    }


    void Serialization::SaveParameters() const
    {
        router::Settings settings = transport_router_.GetSettings();
        auto& parameters = *base_.mutable_parameters();
        parameters.set_bus_wait_time(settings.bus_wait_time);
        parameters.set_bus_velocity(settings.bus_velocity);
    }

    void Serialization::LoadParameters()
    {
        router::Settings result;
        const auto& parameters = base_.parameters();
        result.bus_wait_time = parameters.bus_wait_time();
        result.bus_velocity = parameters.bus_velocity();
        transport_router_.SetSettings(result);
    }
}