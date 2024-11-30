#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "Station.h"
#include "Trip.h"

namespace sdtmaps {

struct Route {
    std::vector<QString> station_ids;
    int transport_id;
    std::vector<Trip> trips;
};

} // namespace sdtmaps

#endif //ROUTE_H
