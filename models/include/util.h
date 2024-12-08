#ifndef MODELS_UTIL_H
#define MODELS_UTIL_H

#include <variant>
#include <QTime>
#include <QString>

#include "../../lib/better-enums/enum.h"

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

    DateTime operator+(int other) const;
    DateTime operator-(int other) const;
    DateTime &operator+=(int other);
    DateTime &operator-=(int other);

    friend std::ostream& operator<<(std::ostream& os, const DateTime &dt);

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

BETTER_ENUM(RouteType, int, TRAM = 0, SUBWAY = 1, RAIL = 2, BUS = 3, FERRY = 4, CABLE_TRAM = 5, AERIAL_LIFT = 6,
    FUNICULAR = 7, TROLLEYBUS = 11, MONORAIL = 12);

struct Route {
    QString name;
    RouteType type;
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

    [[nodiscard]] DateTime startTime() const;
    [[nodiscard]] DateTime endTime() const;
};

struct Journey : std::vector<std::variant<Transfer, Ride>> {
    const Stop *source;
    const Stop *target;
    DateTime startTime{};
    DateTime arrivalTime{};

    friend std::ostream &operator<<(std::ostream &os, const Journey &journey);
};

template<typename T>
std::string to_string(const T &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Journey &journey);

}

#endif // MODELS_UTIL_H
