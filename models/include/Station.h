#ifndef STATION_H
#define STATION_H

#include <QString>

namespace sdtmaps {

struct Station {
    QString id;
    double latitude;
    double longitude;
    QString name;
};

} // namespace sdtmaps

#endif //STATION_H
