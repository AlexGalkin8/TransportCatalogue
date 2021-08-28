#include "transport_catalogue.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <clocale>

using namespace transport_catalogue;

int main()
{
    TransportCatalogue transport_catalogue(RequestFormat::JSON);
    transport_catalogue.AddRequest(std::cin);
}