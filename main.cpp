#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

#include <iostream>
#include <vector>

void test1()
{
    //TransportCatalogue transport_catalogue;

    //size_t num_command = 0;
    //std::vector<std::string> requests_for_input =
    //{
    //    "Stop Tolstopaltsevo: 55.611087, 37.208290",
    //    "Stop Marushkino: 55.595884, 37.209755",
    //    "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
    //    "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka",
    //    "Stop Rasskazovka: 55.632761, 37.333324",
    //    "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700",
    //    "Stop Biryusinka: 55.581065, 37.648390",
    //    "Stop Universam: 55.587655, 37.645687",
    //    "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656",
    //    "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164",
    //};

    //std::vector<std::string>  requests_for_output =
    //{
    //    "Bus 256",
    //    "Bus 750",
    //    "Bus 751"
    //};

    //num_command = requests_for_input.size();
    ////std::cin >> num_command;
    //for (size_t i = 0; i < num_command; i++)
    //{
    //    ReadInputRequest(transport_catalogue, requests_for_input.at(i));
    //}

    //num_command = requests_for_output.size();
    ////std::cin >> num_command;
    //for (size_t i = 0; i < num_command; i++)
    //{
    //    ReadOutRequest(transport_catalogue, requests_for_output.at(i));
    //}
}

void test2()
{
    //TransportCatalogue transport_catalogue;

    //size_t num_command = 0;
    //std::vector<std::string> requests_for_input =
    //{
    //    "Stop A: 55.611087, 37.208230",
    //    "Bus 0 Aa AA: A - B",
    //    "Bus 1: A - A - B - B",
    //    "Bus 2: B - A - B - A",
    //    "Bus 3: A > A > A > A",
    //    "Bus 4: A - A - A - A",
    //    "Stop B: 55.595884, 37.209155",
    //    "Bus 5: A > B > B > A",
    //    "Bus 6: A - B - B - A"
    //};

    //std::vector<std::string>  requests_for_output =
    //{
    //    "Bus 0 Aa AA",
    //    "Bus 1",
    //    "Bus 2",
    //    "Bus 3",
    //    "Bus 4",
    //    "Bus 5",
    //    "Bus 6",
    //    "Bus 7",
    //    "Bus 8",
    //    "Bus 9"
    //};

    //num_command = requests_for_input.size();
    ////std::cin >> num_command;
    //for (size_t i = 0; i < num_command; i++)
    //{
    //    ReadInputRequest(transport_catalogue, requests_for_input.at(i));
    //}

    //num_command = requests_for_output.size();
    ////std::cin >> num_command;
    //for (size_t i = 0; i < num_command; i++)
    //{
    //    ReadOutRequest(transport_catalogue, requests_for_output.at(i));
    //}
}

int main()
{
    TransportCatalogue transport_catalogue;
    ReadInputRequest(transport_catalogue, std::cin);
    ReadOutRequest(transport_catalogue, std::cin);
    //test1();
    //test2();

}