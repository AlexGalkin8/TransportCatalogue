#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

#include <iostream>
#include <vector>
#include <sstream>

void test1();
void test2();
void test3();
void test4();

int main()
{
    //TransportCatalogue transport_catalogue;
    //ReadInputRequest(transport_catalogue, std::cin);
    //ReadOutRequest(transport_catalogue, std::cin);
    //test1();
    //test2();
    //test3();
    test4();
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
    };

    std::vector<std::string>  requests_for_output =
    {
        "Bus 256",
        "Bus 750",
        "Bus 751"
    };

    num_command = requests_for_input.size();
    //std::cin >> num_command;
    std::stringstream is_input;
    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    ReadInputRequest(transport_catalogue, is_input);

    num_command = requests_for_output.size();
    //std::cin >> num_command;
    std::stringstream is_out;
    for (size_t i = 0; i < num_command; i++)
    {
        is_out << requests_for_output.at(i) << std::endl;
    }

    ReadOutRequest(transport_catalogue, is_out);
}

void test2()
{
    TransportCatalogue transport_catalogue;

    size_t num_command = 0;
    std::vector<std::string> requests_for_input =
    {
        "Stop A: 55.611087, 37.208230",
        "Bus 0 Aa AA: A - B",
        "Bus 1:",
        "Bus 2: B - A - B - A",
        "Bus 3: A > A > A > A",
        "Bus 4: A - A - A - A",
        "Stop B: 55.595884, 37.209155",
        "Bus 5: A > B > B > A",
        "Bus 6: A - B - B - A"
    };

    std::vector<std::string>  requests_for_output =
    {
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
    //std::cin >> num_command;
    std::stringstream is_input;
    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    ReadInputRequest(transport_catalogue, is_input);

    num_command = requests_for_output.size();
    //std::cin >> num_command;
    std::stringstream is_out;
    for (size_t i = 0; i < num_command; i++)
    {
        is_out << requests_for_output.at(i) << std::endl;
    }

    ReadOutRequest(transport_catalogue, is_out);
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
        "Stop Prazhskaya: 55.611678, 37.603831"
    };

    std::vector<std::string>  requests_for_output =
    {
        "Bus 256",
        "Bus 750",
        "Bus 751",
        "Stop Samara",
        "Stop Prazhskaya",
        "Stop Biryulyovo Zapadnoye"
    };

    num_command = requests_for_input.size();
    //std::cin >> num_command;
    std::stringstream is_input;
    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    ReadInputRequest(transport_catalogue, is_input);

    num_command = requests_for_output.size();
    //std::cin >> num_command;
    std::stringstream is_out;
    for (size_t i = 0; i < num_command; i++)
    {
        is_out << requests_for_output.at(i) << std::endl;
    }

    ReadOutRequest(transport_catalogue, is_out);
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
        "Stop Prazhskaya: 55.611678, 37.603831"
    };

    std::vector<std::string>  requests_for_output =
    {
        "Bus 256",
        "Bus 750",
        "Bus 751",
        "Stop Samara",
        "Stop Prazhskaya",
        "Stop Biryulyovo Zapadnoye"
    };

    num_command = requests_for_input.size();
    //std::cin >> num_command;
    std::stringstream is_input;
    for (size_t i = 0; i < num_command; i++)
    {
        is_input << requests_for_input.at(i) << std::endl;
    }

    ReadInputRequest(transport_catalogue, is_input);

    num_command = requests_for_output.size();
    //std::cin >> num_command;
    std::stringstream is_out;
    for (size_t i = 0; i < num_command; i++)
    {
        is_out << requests_for_output.at(i) << std::endl;
    }

    ReadOutRequest(transport_catalogue, is_out);
}