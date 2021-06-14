#pragma once

#include "transport_catalogue.h"
#include <iostream>
#include <sstream>

void ReadInputRequest(TransportCatalogue& transport_catalogue, std::istream& is);

void ReadInputRequest(TransportCatalogue& transport_catalogue, std::stringstream& is);
