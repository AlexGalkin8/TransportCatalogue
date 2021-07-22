#include "transport_catalogue.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <clocale>

using namespace transport_catalogue;

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();

int main()
{
    //TransportCatalogue transport_catalogue(RequestFormat::JSON);
    //transport_catalogue.AddRequest(std::cin);
    //test1();
    //test2();
    //test3();
    //test4();
    test5();
    //test6();
    //test7();
}

void test1()
{
    TransportCatalogue transport_catalogue;

    size_t num_command = 0;
    std::vector<std::string> requests_for_input =
    {
        "Stop Tolstopaltsevo: 55.611087, 37.208290",
        "Stop Marushkino: 55.595884, 37.209755",
        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka",
        "Stop Rasskazovka: 55.632761, 37.333324",
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700",
        "Stop Biryusinka: 55.581065, 37.648390",
        "Stop Universam: 55.587655, 37.645687",
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656",
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164",
        "Bus 256",
        "Bus 750",
        "Bus 751"
    };

    num_command = requests_for_input.size();
    std::stringstream is_input("");
    is_input << num_command << std::endl;

    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    transport_catalogue.AddRequest(is_input);
}

void test2()
{
    TransportCatalogue transport_catalogue;

    size_t num_command = 0;
    std::vector<std::string> requests_for_input =
    {
        "Stop A: 55, 37",
        "Bus 0 Aa AA: A - B",
        "Bus 1:",
        "Bus 2: B - A - B - A",
        "Bus 3: A > A > A > A",
        "Bus 4: A - A - A - A",
        "Stop B: -55, -37",
        "Bus 5: A > B > B > A",
        "Bus 6: A - B - B - A",
        "Bus 0 Aa AA",
        "Bus 1",
        "Bus 2",
        "Bus 3",
        "Bus 4",
        "Bus 5",
        "Bus 6",
        "Bus 7",
        "Bus 8",
        "Bus 9"
    };

    num_command = requests_for_input.size();
    std::stringstream is_input("");
    is_input << num_command << std::endl;

    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    transport_catalogue.AddRequest(is_input);
}

void test3()
{
    TransportCatalogue transport_catalogue;

    size_t num_command = 0;
    std::vector<std::string> requests_for_input =
    {
        "Stop Tolstopaltsevo: 55.611087, 37.20829",
        "Stop Marushkino: 55.595884, 37.209755",
        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka",
        "Stop Rasskazovka: 55.632761, 37.333324",
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517",
        "Stop Biryusinka: 55.581065, 37.64839",
        "Stop Universam: 55.587655, 37.645687",
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656",
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164",
        "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye",
        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757",
        "Stop Prazhskaya: 55.611678, 37.603831",
        "Bus 256",
        "Bus 750",
        "Bus 751",
        "Stop Samara",
        "Stop Prazhskaya",
        "Stop Biryulyovo Zapadnoye"
    };

    num_command = requests_for_input.size();
    std::stringstream is_input("");
    is_input << num_command << std::endl;

    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    transport_catalogue.AddRequest(is_input);
}

void test4()
{
    TransportCatalogue transport_catalogue;

    size_t num_command = 0;
    std::vector<std::string> requests_for_input =
    {
        "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino",
        "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino",
        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
        "Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka",
        "Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino",
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam",
        "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam",
        "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya",
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya",
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye",
        "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye",
        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757",
        "Stop Prazhskaya: 55.611678, 37.603831",
        "Bus 256",
        "Bus 750",
        "Bus 751",
        "Stop Samara",
        "Stop Prazhskaya",
        "Stop Biryulyovo Zapadnoye"
    };

    num_command = requests_for_input.size();
    std::stringstream is_input("");
    is_input << num_command << std::endl;

    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    transport_catalogue.AddRequest(is_input);
}

void test5()
{
    std::string line;

    std::ifstream in("input.json");   // окрываем файл для чтения
    std::ofstream out("output.json"); // окрываем файл для записи

    TransportCatalogue transport_catalogue(RequestFormat::JSON);

    if (in.is_open() && out.is_open())
    {
        transport_catalogue.AddRequest(in, out);
        in.seekg(0);
    }

    in.close();
    out.close();
}

void test6()
{
    std::string str = "{ \"base_requests\": [ {\"type\": \"Bus\",\"name\" : \"114\",\"stops\" : [\"Морской вокзал\", \"Ривьерский мост\"] ,\"is_roundtrip\" : false},{\"type\": \"Stop\",\"name\" : \"Ривьерский мост\",\"latitude\" : 43.587795,\"longitude\" : 39.716901,\"road_distances\" : {\"Морской вокзал\": 850}},{\"type\": \"Stop\",\"name\" : \"Морской вокзал\",\"latitude\" : 43.581969,\"longitude\" : 39.719848,\"road_distances\" : {\"Ривьерский мост\": 850}}],\"stat_requests\": [{ \"id\": 1, \"type\" : \"Stop\", \"name\" : \"Ривьерский мост\" },{ \"id\": 2, \"type\" : \"Bus\", \"name\" : \"114\" }]} ";
    std::stringstream str_req(str);

    TransportCatalogue transport_catalogue(RequestFormat::JSON);
    transport_catalogue.AddRequest(str_req);
}

void test7()
{
    std::stringstream test_data("{\n"
        "    \"base_requests\": [\n"
        "      {\n"
        "        \"type\": \"Bus\",\n"
        "        \"name\": \"44\",\n"
        "        \"stops\": [\"A\", \"B\", \"C\", \"D\", \"A\"],\n"
        "        \"is_roundtrip\": true\n"
        "      } ,\n"
        "      {\n"
        "        \"type\": \"Bus\",\n"
        "        \"name\": \"00\",\n"
        "        \"stops\": [],\n"
        "        \"is_roundtrip\": false\n"
        "      } ,\n"
        "      {\n"
        "        \"type\": \"Stop\",\n"
        "        \"name\": \"A\",\n"
        "        \"latitude\": 10,\n"
        "        \"longitude\": 20,\n"
        "        \"road_distances\": {\"B\": -100}\n"
        "      },\n"
        "      {\n"
        "        \"type\": \"Stop\",\n"
        "        \"name\": \"B\",\n"
        "        \"latitude\": 10,\n"
        "        \"longitude\": 30,\n"
        "        \"road_distances\": {\"C\": -100}\n"
        "      },\n"
        "      {\n"
        "        \"type\": \"Stop\",\n"
        "        \"name\": \"C\",\n"
        "        \"latitude\": 20,\n"
        "        \"longitude\": 30,\n"
        "        \"road_distances\": {\"D\": 100}\n"
        "      },\n"
        "      {\n"
        "        \"type\": \"Stop\",\n"
        "        \"name\": \"D\",\n"
        "        \"latitude\": 20,\n"
        "        \"longitude\": 20,\n"
        "        \"road_distances\": {\"A\": -100}\n"
        "      },\n"
        "      {\n"
        "        \"type\": \"Stop\",\n"
        "        \"name\": \"P\",\n"
        "        \"latitude\": 25,\n"
        "        \"longitude\": 15,\n"
        "        \"road_distances\": {\"A\": 70}\n"
        "      }\n"
        "    ],\n"
        "    \"render_settings\": {\n"
        "      \"width\": 200,\n"
        "      \"height\": 200,\n"
        "      \"padding\": 30,\n"
        "      \"stop_radius\": 5,\n"
        "      \"line_width\": 14,\n"
        "      \"bus_label_font_size\": 20,\n"
        "      \"bus_label_offset\": [7, 15],\n"
        "      \"stop_label_font_size\": 20,\n"
        "      \"stop_label_offset\": [7, -3],\n"
        "      \"underlayer_color\": [255,255,255,0.85],\n"
        "      \"underlayer_width\": 3,\n"
        "      \"color_palette\": [\"green\", [255,160,0],\"red\"]\n"
        "    },\n"
        "    \"stat_requests\": [\n"
        "      { \"id\": 1, \"type\": \"Map\" },\n"
        "      { \"id\": 2, \"type\": \"Stop\", \"name\": \"A\" },\n"
        "      { \"id\": 3, \"type\": \"Stop\", \"name\": \"P\" },\n"
        "      { \"id\": 4, \"type\": \"Stop\", \"name\": \"FF\" },\n"
        "      { \"id\": 5, \"type\": \"Bus\", \"name\": \"44\" },\n"
        "      { \"id\": 6, \"type\": \"Bus\", \"name\": \"42\" },\n"
        "      { \"id\": 7, \"type\": \"Bus\", \"name\": \"00\" }\n"
        "    ]\n"
        "  }");

    std::ofstream out("output.json"); // окрываем файл для записи
    TransportCatalogue transport_catalogue(RequestFormat::JSON);
    transport_catalogue.AddRequest(test_data, out);
}