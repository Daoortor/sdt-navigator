#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "../../models/include/Move.h"
#include "../../models/include/TransportSystem.h"

namespace sdtmaps {

typedef std::vector<Move *> Journey;

Journey pathfind(const TransportSystem &transport_system, const QString& start_id, const QString& end_id, QTime start_time);

}

#endif //ROUTE_H
