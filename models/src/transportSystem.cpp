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
        throw std::runtime_error("Failed to open " + filename.toStdString());
    }
    QString json = file.readAll();
    file.close();

    Document document;
    document.Parse(json.toUtf8().constData());
    if (document.HasParseError()) {
        throw std::runtime_error("Failed to parse " + filename.toStdString());
    }

    return document;
}

}

TransportSystem::TransportSystem(const QDir &sourceDir) {
    std::cout << "Loading transport system from " << sourceDir.absolutePath().toStdString() << std::endl;

    auto json_stations = details::parseJsonFile(sourceDir.filePath("stations.json"));
    auto json_transfers = details::parseJsonFile(sourceDir.filePath("transfers.json"));
    auto json_trips = details::parseJsonFile(sourceDir.filePath("trips.json"));

    for (auto &stopData : json_stations.GetArray()) {
        Stop stop;
        stop.name = QString::fromUtf8(stopData[0].GetString());
        stop.id = QString::fromUtf8(stopData[1].GetString());
        stops.push_back(stop);
    }
    cout << "Loaded " << json_stations.Size() << " stops" << endl;
    
    // for (auto &transferData : json_transfers.GetArray()) {
    //     Transfer transfer(
    //         &stops[transferData[0].GetInt()],
    //         &stops[transferData[1].GetInt()],
    //         transferData[2].GetInt()
    //     );
    //     transfers.push_back(transfer);
    // }
    // cout << "Loaded " << json_transfers.Size() << " transfers" << endl;
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
