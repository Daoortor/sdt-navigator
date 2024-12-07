#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <optional>

#include "../../models/include/transportSystem.h"

namespace sdtmaps {

const QDateTime MAX_DATETIME = QDateTime(QDate(3000, 1, 1), QTime(0, 0));

struct dpEntry {
    DateTime optimalTime = MAX_DATETIME;
    Transfer *lastTransfer = nullptr;
    Ride lastRide{};
};

std::optional<Journey> pathfind(const TransportSystem &transportSystem, const QString& sourceId, const QString& targetId, const DateTime &initDateTime, size_t
                                maxChanges = 0);

}

#endif //ROUTE_H
