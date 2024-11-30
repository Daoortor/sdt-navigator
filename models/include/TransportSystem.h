#ifndef TRANSPORTSYSTEM_H
#define TRANSPORTSYSTEM_H

#include <QDir>

#include "Route.h"
#include "Station.h"
#include "Transfer.h"

namespace sdtmaps {

struct TransportSystem {
    std::vector<Station> stations;
    std::vector<Route> routes;
    std::vector<Transfer> transfers;

    explicit TransportSystem(const QDir &data_dir);
};

}

#endif //TRANSPORTSYSTEM_H
