#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <optional>

#include "../../models/include/transportSystem.h"

namespace sdtmaps {

const QDateTime MAX_DATETIME = QDateTime(QDate(3000, 1, 1), QTime(0, 0));

struct dpEntry {
    QDateTime optimalTime = MAX_DATETIME;
    Transfer *lastTransfer = nullptr;
    Ride lastRide;
};

std::optional<Journey> pathfind(const TransportSystem &transportSystem, const QString& startId, const QString& endId, const QDateTime &startDateTime);

}

#endif //ROUTE_H
