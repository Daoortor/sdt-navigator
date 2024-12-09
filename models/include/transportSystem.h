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
    std::vector<std::pair<Route *, Stop **>> stopRoutes; // Routes passing through each stop, each together with corresponding routeStops entry

    TransportSystem() = default;
    // Construct a transport system from JSON data
    explicit TransportSystem(const QDir &sourceDir);
    // Check if all pointers point inside corresponding containers
    [[nodiscard]] bool isValid() const;
    // Find stop by id (to be optimized w/search structures)
    [[nodiscard]] const Stop *getStopById(const QString &stopId) const;
    // Find stop by name (to be optimized w/search structures)
    [[nodiscard]] const Stop *getStopByName(const QString &stopName) const;
    // Find stops by substring (to be optimized w/search structures)
    [[nodiscard]] std::vector<Stop *> getStopsBySubstring(const QString &substring) const;
    [[nodiscard]] QDate getStartDate() const;
};

}

#endif //TRANSPORTSYSTEM_H
