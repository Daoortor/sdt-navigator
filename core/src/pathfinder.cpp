#include "../include/pathfinder.h"
#include <utility>

namespace sdtmaps {

std::optional<Journey> pathfind(const TransportSystem &transportSystem, const QString& startId, const QString& endId, const QDateTime &startDateTime) {
    const Stop *start = transportSystem.getStop(startId);
    const Stop *end = transportSystem.getStop(endId);
    if (!start || !end) {
        return std::nullopt;
    }
    // t_k(p_i)
    std::vector dp(1, std::vector<dpEntry>(transportSystem.stops.size()));
    dp[0][start - &transportSystem.stops[0]] = {startDateTime};
    std::vector marked = {start};
    for (int k = 1; !marked.empty(); k++) {
        dp.emplace_back(dp[k-1]);
        std::vector<std::pair<Route *, Stop **>> routesToRelax;
        std::vector<std::pair<Route *, Stop **> *> entries(transportSystem.routes.size(), nullptr);
        for (auto markedStop : marked) {
            for (auto routeEntry = markedStop->routes; routeEntry != &markedStop->routes[markedStop->routeCount]; routeEntry++) {
                Route *servingRoute = routeEntry->first;
                Stop **stopRoutesEntry = routeEntry->second;
                auto entry = &entries[servingRoute - &*transportSystem.routes.begin()];
                if (!*entry) {
                    routesToRelax.emplace_back(servingRoute, stopRoutesEntry);
                    *entry = &*routesToRelax.end() - 1;
                    continue;
                }
                if (stopRoutesEntry < (*entry)->second) {
                    (*entry)->second = stopRoutesEntry;
                }
            }
        }
        routesToRelax[0].first - &*transportSystem.routes.begin();
        marked.clear();
        std::vector isMarked(transportSystem.stops.size(), false);
        for (auto &[routeToRelax, initStop] : routesToRelax) {
            std::vector debug(routeToRelax->stopTimes, routeToRelax->stopTimes + routeToRelax->stopCount * routeToRelax->tripCount);
            StopTime *currentTrip = nullptr;
            Stop **boardedStop = nullptr;
            for (Stop **currentStop = initStop; currentStop != routeToRelax->stops + routeToRelax->stopCount; currentStop++) {
                size_t currentStopRouteIndex = currentStop - routeToRelax->stops;
                size_t currentStopStopsIndex = *currentStop - &*transportSystem.stops.begin();
                StopTime *lastTrip = routeToRelax->stopTimes + routeToRelax->stopCount * (routeToRelax->tripCount - 1);
                if (!currentTrip && dp[k-1][currentStopStopsIndex].optimalTime <= (lastTrip + currentStopRouteIndex)->departureTime) {
                    currentTrip = lastTrip;
                    boardedStop = currentStop;
                }
                if (currentTrip) {
                    while (currentTrip != routeToRelax->stopTimes &&
                        dp[k-1][currentStopStopsIndex].optimalTime <=
                        (currentTrip - routeToRelax->stopCount + currentStopRouteIndex)->departureTime) {
                        currentTrip -= routeToRelax->stopCount;
                        boardedStop = currentStop;
                    }
                    StopTime *currentStopTime = currentTrip + currentStopRouteIndex;
                    dpEntry *currentStopOpt = &dp[k-1][*currentStop - &*transportSystem.stops.begin()];
                    dpEntry *endStopOpt = &dp[k-1][end - &*transportSystem.stops.begin()];
                    if (currentStopTime->arrivalTime < std::min(currentStopOpt->optimalTime, endStopOpt->optimalTime)) {
                        dp[k][currentStopStopsIndex] = {
                            currentStopTime->arrivalTime,
                            nullptr,
                            {
                                routeToRelax,
                                boardedStop,
                                currentStop,
                                currentTrip,
                            }
                        };
                        isMarked[currentStopStopsIndex] = true;
                    }
                }
                for (int i=0; i<isMarked.size(); i++) {
                    if (isMarked[i]) {
                        marked.push_back(&*transportSystem.stops.begin() + i);
                    }
                }
            }
        }
        for (auto markedStop : marked) {
            for (Transfer *transfer=markedStop->transfers; transfer!=&*transportSystem.transfers.end() && transfer->start == markedStop; transfer++) {
                size_t startIndex = transfer->end - &*transportSystem.stops.begin();
                size_t endIndex = transfer->end - &*transportSystem.stops.begin();
                if (dp[k][startIndex].optimalTime.addSecs(transfer->time) < dp[k][endIndex].optimalTime) {
                    dp[k][endIndex] = {
                        dp[k][startIndex].optimalTime.addSecs(transfer->time),
                        transfer,
                        nullptr
                    };
                }
            }
        }
    }
    size_t lastLayer = dp.size() - 1;
    Journey result;
    Stop *cur = const_cast<Stop *>(end);
    while (cur != start) {
        dpEntry curEntry = dp[lastLayer][cur - &*transportSystem.stops.begin()];
        if (curEntry.lastRide.route) {
            result.emplace_back(std::in_place_type_t<Ride>(), curEntry.lastRide);
            cur = *curEntry.lastRide.firstStop;
        } else if (curEntry.lastTransfer) {
            result.emplace_back(std::in_place_type_t<Transfer>(), *curEntry.lastTransfer);
        } else {
            return std::nullopt;
        }
    }
    std::ranges::reverse(result);
    return result;
}

}
