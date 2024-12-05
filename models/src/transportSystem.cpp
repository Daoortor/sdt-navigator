#include "../include/transportSystem.h"

#include <iostream>

#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

namespace sdtmaps {

namespace details {

Document parseJsonFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open JSON file " + filename.toStdString());
    }
    QString json = file.readAll();
    file.close();

    Document document;
    document.Parse(json.toUtf8().constData());
    if (document.HasParseError()) {
        throw std::runtime_error("Failed to parse JSON file " + filename.toStdString());
    }

    return document;
}

}

TransportSystem::TransportSystem(const QDir &sourceDir) {
    std::cout << "Loading transport system from directory \"" << sourceDir.dirName().toStdString() << "\"..." << std::endl;

    auto jsonStations = details::parseJsonFile(sourceDir.filePath("stations.json"));
    auto jsonTransfers = details::parseJsonFile(sourceDir.filePath("transfers.json"));
    auto jsonTrips = details::parseJsonFile(sourceDir.filePath("trips.json"));

    map<QString, Stop *> stopById;
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
    }
    cout << "Stops loaded" << endl;
    
    map<Stop *, std::vector<Route *>> routesByStop;
    map<Route *, size_t> stopTimesIndexByRoute;
    map<Route *, size_t> routeStopsIndexByRoute;
    routes.reserve(jsonTrips.Size());
    cout << "Loading routes (" << jsonTrips.Size() << ")..." << endl;
    for (auto &routeData : jsonTrips.GetArray()) {
        size_t firstAddedStop = routeStops.size();
        routeStops.reserve(routeStops.size() + routeData[0].Size());
        for (auto &stop_id : routeData[0].GetArray()) {
            routeStops.push_back(stopById[QString::fromUtf8(stop_id.GetString())]);
        }

        size_t firstAddedStopTime = stopTimes.size();
        // Not reserving space for stopTimes, because it's complicated and won't save much time anyway
        for (auto &trip : routeData[1].GetArray()) {
            for (auto &stopTimeData : trip.GetArray()) {
                stopTimes.emplace_back(
                    QDateTime::fromSecsSinceEpoch(stopTimeData[0].GetInt()),
                    QDateTime::fromSecsSinceEpoch(stopTimeData[1].GetInt())
                );
            }
        }

        routes.emplace_back(
            static_cast<int>(routeData[1].Size()),
            static_cast<int>(routeData[0].Size()),
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
    
    map<Stop *, std::vector<Transfer>> transfersByStop;
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
            throw std::runtime_error("Stop is not assigned to any route");
            // if (stop.id == "000000037041") {
            //     cout << "Setting stop.routes to &*stopRoutes.end()" << endl;
            // }
            // stop.routes = &*stopRoutes.end();
        }
    }
    cout << "Transport system loaded" << endl;
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
