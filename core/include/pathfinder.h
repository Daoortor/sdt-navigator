#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "../../models/include/transportSystem.h"

namespace sdtmaps {

Journey pathfind(const TransportSystem &transport_system, const QString& start_id, const QString& end_id, QTime start_time);

}

#endif //ROUTE_H
