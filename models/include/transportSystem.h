#ifndef TRANSPORTSYSTEM_H
#define TRANSPORTSYSTEM_H

#include "util.h"
#include <vector>
#include <QDir>

namespace sdtmaps {

struct TransportSystem {
    std::vector<Route> routes;
    std::vector<StopTime> stopTimes;
    std::vector<Stop *> routeStops;
    std::vector<Stop> stops;
    std::vector<Transfer> transfers;
    std::vector<Route *> stopRoutes;

    TransportSystem() = default;
    // Construct a transport system from data (.json or .csv? @npanuhin?)
    explicit TransportSystem(const QDir &sourceDir);
    // Check if all pointers point inside corresponding containers
    [[nodiscard]] bool isValid() const;
};

}

#endif //TRANSPORTSYSTEM_H
