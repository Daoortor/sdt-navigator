#include "../include/pathfinder.h"
#include <utility>
#include <iostream>

namespace sdtmaps {

std::optional<Journey> pathfind(const TransportSystem &transportSystem, const QString& sourceId, const QString& targetId, const DateTime &initDateTime
    , size_t maxChanges) {
    const Stop *source = transportSystem.getStopById(sourceId);
    const Stop *target = transportSystem.getStopById(targetId);
    // If any of the stops was not found
    if (!source || !target) {
        return std::nullopt;
    }
    size_t sourceIndex = source - &*transportSystem.stops.begin();
    size_t targetIndex = target - &*transportSystem.stops.begin();
    // kth layer - optimal trips using <= k rides
    std::vector<dpEntry> curLayer(transportSystem.stops.size());
    std::vector<dpEntry> prevLayer(transportSystem.stops.size());
    curLayer[sourceIndex] = {initDateTime};
    // Stops that were updated during last round
    std::vector isMarked(transportSystem.stops.size(), false);
    isMarked[sourceIndex] = true;
    // Any updates during this round?
    bool shouldContinue = true;
    for (int k = 0; shouldContinue && (maxChanges == 0 || k <= maxChanges); k++) {
        prevLayer = curLayer;
        // Routes passing through marked stops + first marked stop on each route
        std::vector<std::pair<Route *, Stop **>> routesToRelax;
        std::vector<std::pair<Route *, Stop **> *> routesToRelaxEntries(transportSystem.routes.size(), nullptr);
        shouldContinue = false;
        for (int stopIndex=0; stopIndex<transportSystem.stops.size(); stopIndex++) {
            if (!isMarked[stopIndex]) {
                continue;
            }
            shouldContinue = true;
            const Stop *markedStop = &transportSystem.stops[stopIndex];
            for (auto routeEntry = markedStop->routes;
                    routeEntry != markedStop->routes + markedStop->routeCount; routeEntry++) {
                Route *route = routeEntry->first;
                size_t routeIndex = route - &*transportSystem.routes.begin();
                Stop **markedStopInRoute = routeEntry->second;
                auto entry = routesToRelaxEntries[routeIndex];
                // If route not added yet
                if (!entry) {
                    routesToRelax.emplace_back(route, markedStopInRoute);
                // Update first marked stop
                } else if (markedStopInRoute < entry->second) {
                    entry->second = markedStopInRoute;
                }
            }
        }
        isMarked = std::vector(transportSystem.stops.size(), false);
        for (auto [routeToRelax, initStop] : routesToRelax) {
            StopTime *lastTrip = routeToRelax->stopTimes + routeToRelax->stopCount * (routeToRelax->tripCount - 1);
            // Earliest trip we can take right now; nullptr if cannot take anything
            StopTime *currentTrip = nullptr;
            // Stop at which we boarded currentTrip; nullptr if currentTrip = nullptr
            Stop **boardedStop = nullptr;
            for (Stop **currentStop = initStop; currentStop != routeToRelax->stops + routeToRelax->stopCount; currentStop++) {
                size_t currentStopIndexInRoute = currentStop - routeToRelax->stops;
                size_t currentStopIndexInStops = *currentStop - &*transportSystem.stops.begin();
                // If we haven't boarded any trip yet, try to board lastTrip
                if (!currentTrip && prevLayer[currentStopIndexInStops].optimalTime <= (lastTrip + currentStopIndexInRoute)->departureTime) {
                    currentTrip = lastTrip;
                    boardedStop = currentStop;
                }
                if (currentTrip) {
                    StopTime *currentStopTime = currentTrip + currentStopIndexInRoute;
                    dpEntry *currentStopOpt = &prevLayer[currentStopIndexInStops];
                    dpEntry *targetOpt = &prevLayer[targetIndex];
                    // Relax dp; target pruning - ignore routes after earliest known arrival time at target
                    if (currentStopTime->arrivalTime < std::min(currentStopOpt->optimalTime, targetOpt->optimalTime)) {
                        curLayer[currentStopIndexInStops] = {
                            currentStopTime->arrivalTime,
                            nullptr,
                            {
                                routeToRelax,
                                boardedStop,
                                currentStop,
                                currentTrip + (boardedStop - routeToRelax->stops),
                            }
                        };
                        isMarked[currentStopIndexInStops] = true;
                    }
                    // Switch to previous trip while we can
                    while (currentTrip != routeToRelax->stopTimes &&
                            prevLayer[currentStopIndexInStops].optimalTime <=
                            (currentTrip - routeToRelax->stopCount + currentStopIndexInRoute)->departureTime) {
                        currentTrip -= routeToRelax->stopCount;
                        boardedStop = currentStop;
                    }
                }
            }
        }
        for (int stopIndex=0; stopIndex<transportSystem.stops.size(); stopIndex++) {
            if (!isMarked[stopIndex]) {
                continue;
            }
            const Stop *markedStop = &transportSystem.stops[stopIndex];
            // Relax along all transfers that are incident to markedStop
            for (Transfer *transfer=markedStop->transfers; transfer!=&*transportSystem.transfers.end() && transfer->start == markedStop; transfer++) {
                size_t transferStartIndex = transfer->start - &*transportSystem.stops.begin();
                size_t transferEndIndex = transfer->end - &*transportSystem.stops.begin();
                // Relax dp; target pruning - ignore routes after earliest known arrival time at target
                if (curLayer[transferStartIndex].optimalTime + transfer->time < curLayer[transferEndIndex].optimalTime) {
                    curLayer[transferEndIndex] = {
                        curLayer[transferStartIndex].optimalTime + transfer->time,
                        transfer,
                        nullptr
                    };
                    isMarked[transferEndIndex] = true;
                }
            }
        }
    }
    // Restore journey path using backtracking
    Journey result;
    result.source = source;
    result.target = target;
    Stop *cur = const_cast<Stop *>(target);
    result.arrivalTime = curLayer[targetIndex].optimalTime;
    DateTime currentTime = result.arrivalTime;
    while (cur != source) {
        size_t curStopsIndex = cur - &*transportSystem.stops.begin();
        dpEntry curEntry = curLayer[curStopsIndex];
        if (curEntry.lastRide.route) {
            result.emplace_back(std::in_place_type_t<Ride>(), curEntry.lastRide);
            cur = *curEntry.lastRide.firstStop;
            currentTime = curEntry.lastRide.startTime();
        } else if (curEntry.lastTransfer) {
            result.emplace_back(std::in_place_type_t<Transfer>(), *curEntry.lastTransfer);
            cur = curEntry.lastTransfer->start;
            currentTime -= curEntry.lastTransfer->time;
        } else {
            return std::nullopt;
        }
    }
    result.startTime = currentTime;
    std::ranges::reverse(result);
    return result;
}

}
