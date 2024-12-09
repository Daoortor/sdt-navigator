#ifndef TRANSPORTSYSTEM_H
#define TRANSPORTSYSTEM_H

#include "util.h"
#include <vector>
#include <QDir>

#include "../../core/include/substring-finder.h"

namespace sdtmaps {

struct TransportSystem {
    std::vector<Route> routes;
    std::vector<StopTime> stopTimes;
    std::vector<Stop *> routeStops;
    std::vector<Stop> stops;
    std::vector<Transfer> transfers;
    std::vector<std::pair<Route *, Stop **>> stopRoutes; // Routes passing through each stop, each together with corresponding routeStops entry
    std::map<QString, Stop *> stopById;
    std::map<QString, Stop *> stopByName;
    SuffixAutomaton stopNamesAutomaton;

    TransportSystem() = default;
    // Construct a transport system from JSON data
    explicit TransportSystem(const QDir &sourceDir);
    void initStopNamesAutomaton();
    // Check if all pointers point inside corresponding containers
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] const Stop *getStopById(const QString &stopId) const;
    // Find stop by name
    [[nodiscard]] const Stop *getStopByName(const QString &stopName) const;
    // Find stops by substring, order by number of routes passing through (to be optimized w/search structures)
    [[nodiscard]] std::vector<const Stop *> getStopsBySubstring(const QString &substring) const;
};

}

#endif //TRANSPORTSYSTEM_H
