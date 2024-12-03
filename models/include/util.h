#ifndef STOPTIME_H
#define STOPTIME_H

#include <variant>
#include <QTime>
#include <QString>

namespace sdtmaps {

struct StopTime {
    QTime arrivalTime;
    QTime departureTime;
};

// Public transport route with a fixed sequence of stops
struct Route;

struct Transfer;

struct Stop {
    QString name;
    QString id;
    Transfer *transfers; // pointer to TransportSystem.transfers[i]; &*transfers.end() if none
    Route **routes; // pointer to TransportSystem.stopRoutes[i]
};

struct Route {
    int tripCount;
    int stopCount;
    StopTime *stopTimes; // pointer to TransportSystem.stopTimes[i]
    Stop **stops; // pointer to TransportSystem.routeStops[i]
};

// Transfer between two close stations by foot
struct Transfer {
    Stop *start; // pointer to TransportSystem.stops[i]
    Stop *end; // pointer to TransportSystem.stops[i]
    int time; // minimum time required for transfer
};

// Public transport ride at a specific time from stop A to stop B
struct Ride {
    Route *route; // pointer to TransportSystem.routes[i]
    Stop **firstStop; // pointer to TransportSystem.routeStops;
    StopTime *firstStopTime; // pointer to TransportSystem.stopTimes[i]
    int rideLength; // number of stations travelled
};

typedef std::vector<std::variant<Transfer, Ride>> Journey;

}

#endif //STOPTIME_H
