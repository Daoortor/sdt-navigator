#ifndef TRIP_H
#define TRIP_H

#include <QTime>

#include "Move.h"

namespace sdtmaps {

struct Trip : public Move {
    struct StationTime {
        QTime arrivalTime;
        QTime departureTime;
    };
    std::vector<StationTime> times;
};

} // namespace sdtmaps

#endif //TRIP_H
