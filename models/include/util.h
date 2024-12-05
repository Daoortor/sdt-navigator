#ifndef STOPTIME_H
#define STOPTIME_H

#include <variant>
#include <QTime>
#include <QString>

namespace sdtmaps {

class DateTime {
public:
    DateTime() = default;
    DateTime(long long timestamp);
    DateTime(const QDateTime &qdt);
    
    bool operator==(const DateTime &other) const;
    bool operator!=(const DateTime &other) const;
    bool operator<(const DateTime &other) const;
    bool operator<=(const DateTime &other) const;
    bool operator>(const DateTime &other) const;
    bool operator>=(const DateTime &other) const;
    
    DateTime addSecs(int secs) const;

private:
    long long timestamp;
};

struct StopTime {
    DateTime arrivalTime;
    DateTime departureTime;
};

// Public transport route with a fixed sequence of stops
struct Route;

struct Transfer;

struct Stop {
    QString name;
    QString id;
    Transfer *transfers; // pointer to TransportSystem.transfers[i]; &*transfers.end() if none
    std::pair<Route *, Stop **> *routes; // pointer to TransportSystem.stopRoutes[i]
    int routeCount;
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
    Stop **lastStop;
    StopTime *firstStopTime; // pointer to TransportSystem.stopTimes[i]
};

typedef std::vector<std::variant<Transfer, Ride>> Journey;

}

#endif //STOPTIME_H
