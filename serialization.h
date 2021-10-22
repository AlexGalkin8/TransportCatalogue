#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include "map_renderer.pb.h"
#include "transport_catalogue.pb.h"

#include <filesystem>

namespace transport_catalogue::serialization
{
    using Path = std::filesystem::path;

    class Serialization
    {
    public:
        Serialization(database::TransportCatalogue& database,
            renderer::MapRenderer& map_renderer,
            router::TransportRouter& transport_router);


        // Задаёт параметры для сериализации (сохраняет путь к файлу)
        void SetSetting(const Path& path_to_base);

        // Сериализует данные в файл
        void CreateBase();

        // Десириализует данные из файла и заполняет базу данных
        void AccessBase();

        proto_renderer::Map LoadMap() const;

    private:
        proto_tc::Stop SaveStop(const objects::Stop& stop) const;

        proto_tc::StopsDistance SaveStopsDistance(std::shared_ptr<objects::Stop> from,
            std::shared_ptr<objects::Stop> to, uint64_t distance) const;

        proto_tc::Bus SaveBus(const objects::Bus& bus) const;


        void SaveStops();

        void SaveStopsDistance();

        void SaveBuses();


        void LoadStop(const proto_tc::Stop& stop);

        void LoadStopsDistance(const proto_tc::StopsDistance& from_to_distance);

        void LoadBus(const proto_tc::Bus& bus);


        void LoadStops();

        void LoadStopsDistance();

        void LoadBuses();


        void SaveMap() const;


        void  SaveParameters() const;

        void LoadParameters();

    private:
        Path path_to_base_;
        database::TransportCatalogue& database_;
        renderer::MapRenderer& map_renderer_;
        router::TransportRouter& transport_router_;
        mutable proto_tc::TransportCatalogue base_;
    };
}