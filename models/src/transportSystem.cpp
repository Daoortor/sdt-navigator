#include "../include/transportSystem.h"

#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;

namespace sdtmaps {

namespace details {

Document parseJsonFile(const QString &filename) {
    FILE* file = fopen(filename.toUtf8().constData(), "r");
    if (!file) {
        throw runtime_error("Failed to open JSON file " + filename.toStdString());
    }

    char readBuffer[65536];
    FileReadStream is(file, readBuffer, sizeof(readBuffer));
    
    Document document;
    document.ParseStream(is);
    
    fclose(file);
   
    if (document.HasParseError()) {
        throw runtime_error("Failed to parse JSON file " + filename.toStdString());
    }
    
    return document;
}

}

TransportSystem::TransportSystem(const QDir &sourceDir) {
    cout << "Loading transport system from directory \"" << sourceDir.dirName().toStdString() << "\"..." << endl;

    auto jsonStations = details::parseJsonFile(sourceDir.filePath("stations.json"));
    auto jsonTransfers = details::parseJsonFile(sourceDir.filePath("transfers.json"));
    auto jsonTrips = details::parseJsonFile(sourceDir.filePath("trips.json"));

    stops.reserve(jsonStations.Size());
    cout << "Loading stops (" << jsonStations.Size() << ")..." << endl;
    for (auto &stopData : jsonStations.GetArray()) {
        stops.emplace_back(
            QString::fromUtf8(stopData[0].GetString()),
            QString::fromUtf8(stopData[1].GetString()),
            nullptr,
            nullptr,
            0
        );
        stopById[stops.back().id] = &stops.back();  // `stops` has its size finalized
        stopByName[stops.back().name] = &stops.back();
    }
    cout << "Stops loaded" << endl;
    
    map<Stop *, vector<Route *>> routesByStop;
    map<Route *, size_t> stopTimesIndexByRoute;
    map<Route *, size_t> routeStopsIndexByRoute;
    routes.reserve(jsonTrips.Size());
    cout << "Loading routes (" << jsonTrips.Size() << ")..." << endl;
    for (auto &routeData : jsonTrips.GetArray()) {
        size_t firstAddedStop = routeStops.size();
        routeStops.reserve(routeStops.size() + routeData[2].Size());
        for (auto &stop_id : routeData[2].GetArray()) {
            routeStops.push_back(stopById[QString::fromUtf8(stop_id.GetString())]);
        }

        size_t firstAddedStopTime = stopTimes.size();
        // Not reserving space for stopTimes, because it's complicated and won't save much time anyway
        for (auto &trip : routeData[3].GetArray()) {
            for (auto &stopTimeData : trip.GetArray()) {
                stopTimes.emplace_back(
                    stopTimeData[0].GetInt64(),
                    stopTimeData[1].GetInt64()
                );
            }
        }

        routes.emplace_back(
            routeData[0].GetString(),
            routeData[1].GetInt(),
            static_cast<int>(routeData[3].Size()),
            static_cast<int>(routeData[2].Size()),
            nullptr,
            nullptr
        );
        stopTimesIndexByRoute[&routes.back()] = firstAddedStopTime;  // `routes` has its size finalized
        routeStopsIndexByRoute[&routes.back()] = firstAddedStop;  // `routes` has its size finalized

        for (size_t i = firstAddedStop; i < routeStops.size(); i++) {
            routesByStop[routeStops[i]].push_back(&routes.back());  // `routes` has its size finalized
        }
    }
    for (auto &[route, firstAddedStopTime] : stopTimesIndexByRoute) {
        route->stopTimes = &stopTimes[firstAddedStopTime];  // `stopTimes` has its size finalized
    }
    for (auto &[route, firstAddedStop] : routeStopsIndexByRoute) {
        route->stops = &routeStops[firstAddedStop];  // `routeStops` has its size finalized
    }

    map<Stop *, size_t> stopRoutesIndexByStop;
    cout << "Assigning routes to stops..." << endl;
    for (auto &[stop, curStopRoutes] : routesByStop) {
        size_t firstAddedRoute = stopRoutes.size();
        for (Route *route : curStopRoutes) {
            stopRoutes.emplace_back(route, route->stops);
        }
        stopRoutesIndexByStop[stop] = firstAddedRoute;
        stop->routeCount = static_cast<int>(curStopRoutes.size());
    }
    for (auto &[stop, firstAddedRoute] : stopRoutesIndexByStop) {
        stop->routes = &stopRoutes[firstAddedRoute];  // `stopRoutes` has its size finalized
    }
    cout << "Routes loaded" << endl;

    map<Stop *, vector<Transfer>> transfersByStop;
    cout << "Loading transfers (" << jsonTransfers.Size() << ")..." << endl;
    for (auto &transferData : jsonTransfers.GetArray()) {
        Stop *startStop = stopById[QString::fromUtf8(transferData[0].GetString())];
        Stop *endStop = stopById[QString::fromUtf8(transferData[1].GetString())];
        transfersByStop[startStop].emplace_back(
            startStop,
            endStop,
            transferData[2].GetInt()
        );
    }

    transfers.reserve(jsonTransfers.Size());
    for (auto &[stop, curStopTransfers] : transfersByStop) {
        size_t firstAddedTransfer = transfers.size();
        for (Transfer &transfer : curStopTransfers) {
            transfers.push_back(transfer);
        }
        stop->transfers = &transfers[firstAddedTransfer];  // `transfers` has its size finalized
    }
    cout << "Transfers loaded" << endl;

    cout << "Final modifications..." << endl;
    for (auto &stop : stops) {
        if (stop.transfers == nullptr) {
            stop.transfers = &*transfers.end();  // `transfers` has its size finalized
        }
        if (stop.routes == nullptr) {
            // throw runtime_error("Stop is not assigned to any route: " + stop.id.toStdString());
            stop.routes = &*stopRoutes.end();  // `stopRoutes` has its size finalized
        }
    }
    cout << "Building stops index..." << endl;
    initStopNamesAutomaton();
    cout << "Transport system loaded" << endl;
}

void TransportSystem::initStopById() {
    for (auto &stop : stops) {
        stopById[stop.id] = &stop;
    }
}

void TransportSystem::initStopByName() {
    for (auto &stop : stops) {
        stopByName[stop.name] = &stop;
    }
}

void TransportSystem::initStopNamesAutomaton() {
    std::vector<QString> stationNames;
    stationNames.reserve(stops.size());
    for (const auto &stop : stops) {
        stationNames.push_back(stop.name);
    }
    stopNamesAutomaton = SuffixAutomaton(stationNames);
}

bool TransportSystem::isValid() const {
    bool routesAreValid = ranges::all_of(routes.begin(), routes.end(), [&](const Route &route) {
        return route.stopTimes >= &*stopTimes.begin() && route.stopTimes < &*stopTimes.end() &&
            route.stops >= &*routeStops.begin() && route.stops < &*routeStops.end();
    });
    bool routeStopsAreValid = ranges::all_of(routeStops.begin(), routeStops.end(), [&](const Stop *routeStop) {
        return routeStop >= &*stops.begin() && routeStop < &*stops.end();
    });
    bool stopsAreValid = ranges::all_of(stops.begin(), stops.end(), [&](const Stop &stop) {
        return stop.transfers >= &*transfers.begin() && stop.transfers <= &*transfers.end() &&
            stop.routes >= &*stopRoutes.begin() && stop.routes <= &*stopRoutes.end();
    });
    bool transfersAreValid = ranges::all_of(transfers.begin(), transfers.end(), [&](const Transfer &transfer) {
        return transfer.start >= &*stops.begin() && transfer.start < &*stops.end() &&
            transfer.end >= &*stops.begin() && transfer.end < &*stops.end();
    });
    bool stopRoutesAreValid = ranges::all_of(stopRoutes.begin(), stopRoutes.end(), [&](const pair<Route *, Stop **> &entry) {
        Route *stopRoute = entry.first;
        return stopRoute >= &*routes.begin() && stopRoute < &*routes.end();
    });
    return routesAreValid && routeStopsAreValid && stopsAreValid && transfersAreValid && stopRoutesAreValid;
}

const Stop *TransportSystem::getStopById(const QString &stopId) const {
    auto it = stopById.find(stopId);
    return it == stopById.end() ? nullptr : it->second;
}

const Stop *TransportSystem::getStopByName(const QString &stopName) const {
    auto it = stopByName.find(stopName);
    return it == stopByName.end() ? nullptr : it->second;
}

std::vector<const Stop *> TransportSystem::getStopsBySubstring(const QString &substring) const {
    std::vector<QString> stopNamesFound = stopNamesAutomaton.findAllStringsContaining(substring);
    std::vector<const Stop *> stopsFound;
    stopsFound.reserve(stopNamesFound.size());
    for (const auto &stopName : stopNamesFound) {
        stopsFound.push_back(getStopByName(stopName));
    }
    std::ranges::sort(stopsFound, [](const Stop *stop1, const Stop *stop2) {
        return (stop1->routeCount == stop2->routeCount) ? stop1->name < stop2->name : stop1->routeCount > stop2->routeCount;
    });
    return stopsFound;
}

}
