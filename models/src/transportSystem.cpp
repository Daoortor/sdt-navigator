#include "../include/transportSystem.h"

namespace sdtmaps {

TransportSystem::TransportSystem(const QDir &sourceDir) {
    // TODO(data loader by @npanukhin)
}

bool TransportSystem::isValid() const {
    bool routesAreValid = std::ranges::all_of(routes.begin(), routes.end(), [&](const Route &route) {
        return route.stopTimes >= &*stopTimes.begin() && route.stopTimes < &*stopTimes.end() &&
            route.stops >= &*routeStops.begin() && route.stops < &*routeStops.end();
    });
    bool routeStopsAreValid = std::ranges::all_of(routeStops.begin(), routeStops.end(), [&](const Stop *routeStop) {
        return routeStop >= &*stops.begin() && routeStop < &*stops.end();
    });
    bool stopsAreValid = std::ranges::all_of(stops.begin(), stops.end(), [&](const Stop &stop) {
        return stop.transfers >= &*transfers.begin() && stop.transfers <= &*transfers.end() &&
            stop.routes >= &*stopRoutes.begin() && stop.routes < &*stopRoutes.end();
    });
    bool transfersAreValid = std::ranges::all_of(transfers.begin(), transfers.end(), [&](const Transfer &transfer) {
        return transfer.start >= &*stops.begin() && transfer.start < &*stops.end() &&
            transfer.end >= &*stops.begin() && transfer.end < &*stops.end();
    });
    bool stopRoutesAreValid = std::ranges::all_of(stopRoutes.begin(), stopRoutes.end(), [&](const std::pair<Route *, Stop **> &entry) {
        Route *stopRoute = entry.first;
        return stopRoute >= &*routes.begin() && stopRoute < &*routes.end();
    });
    return routesAreValid && routeStopsAreValid && stopsAreValid && transfersAreValid && stopRoutesAreValid;
}

const Stop *TransportSystem::getStop(const QString &stopId) const {
    for (auto &stop : stops) {
        if (stop.id == stopId) {
            return &stop;
        }
    }
    return nullptr;
}

}
