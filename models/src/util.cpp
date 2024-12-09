#include "../include/util.h"

namespace sdtmaps {

DateTime::DateTime(long long timestamp) : timestamp(timestamp) {}

DateTime::DateTime(const QDateTime &qdt) : timestamp(qdt.toSecsSinceEpoch()) {}

bool DateTime::operator==(const DateTime &other) const {
    return this->timestamp == other.timestamp;
}

bool DateTime::operator!=(const DateTime &other) const {
    return !(*this == other);
}

bool DateTime::operator<(const DateTime &other) const {
    return this->timestamp < other.timestamp;
}

bool DateTime::operator<=(const DateTime &other) const {
    return this->timestamp <= other.timestamp;
}

bool DateTime::operator>(const DateTime &other) const {
    return this->timestamp > other.timestamp;
}

bool DateTime::operator>=(const DateTime &other) const {
    return this->timestamp >= other.timestamp;
}

DateTime DateTime::operator+(int other) const {
    return {this->timestamp + other};
}

DateTime DateTime::operator-(int other) const {
    return {this->timestamp - other};
}

DateTime &DateTime::operator+=(int other) {
    this->timestamp += other;
    return *this;
}

DateTime &DateTime::operator-=(int other) {
    this->timestamp -= other;
    return *this;
}

QDateTime DateTime::toQDateTime() const {
    return QDateTime::fromSecsSinceEpoch(timestamp);
}

QTextStream& operator<<(QTextStream& os, const DateTime &dt) {
    return os << dt.toQDateTime().toString();
}

DateTime Ride::startTime() const {
    return firstStopTime->departureTime;
}

DateTime Ride::endTime() const {
    return (firstStopTime + (lastStop - firstStop))->arrivalTime;
}

std::map<int, QString> routeTypeNames = {
    {0, "TRAM"},
    {1, "SUBWAY"},
    {2, "RAIL"},
    {3, "BUS"},
    {4, "FERRY"},
    {5, "CABLE_TRAM"},
    {6, "AERIAL_LIFT"},
    {7, "FUNICULAR"},
    {11, "TROLLEYBUS"},
    {12, "MONORAIL"},
    {100, "RAILWAYSERVICE"},
    {101, "HIGHSPEEDRAILSERVICE"},
    {102, "LONGDISTANCETRAINS"},
    {103, "INTERREGIONALRAILSERVICE"},
    {104, "CARTRANSPORTRAILSERVICE"},
    {105, "SLEEPERRAILSERVICE"},
    {106, "REGIONALRAILSERVICE"},
    {107, "TOURISTRAILWAYSERVICE"},
    {108, "RAILSHUTTLE(WITHINCOMPLEX)"},
    {109, "SUBURBANRAILWAY"},
    {110, "REPLACEMENTRAILSERVICE"},
    {111, "SPECIALRAILSERVICE"},
    {112, "LORRYTRANSPORTRAILSERVICE"},
    {113, "ALLRAILSERVICES"},
    {114, "CROSS-COUNTRYRAILSERVICE"},
    {115, "VEHICLETRANSPORTRAILSERVICE"},
    {116, "RACKANDPINIONRAILWAY"},
    {117, "ADDITIONALRAILSERVICE"},
    {200, "COACHSERVICE"},
    {201, "INTERNATIONALCOACHSERVICE"},
    {202, "NATIONALCOACHSERVICE"},
    {203, "SHUTTLECOACHSERVICE"},
    {204, "REGIONALCOACHSERVICE"},
    {205, "SPECIALCOACHSERVICE"},
    {206, "SIGHTSEEINGCOACHSERVICE"},
    {207, "TOURISTCOACHSERVICE"},
    {208, "COMMUTERCOACHSERVICE"},
    {209, "ALLCOACHSERVICES"},
    {400, "URBANRAILWAYSERVICE"},
    {401, "METROSERVICE"},
    {402, "UNDERGROUNDSERVICE"},
    {403, "URBANRAILWAYSERVICE"},
    {404, "ALLURBANRAILWAYSERVICES"},
    {405, "MONORAIL"},
    {700, "BUSSERVICE"},
    {701, "REGIONALBUSSERVICE"},
    {702, "EXPRESSBUSSERVICE"},
    {703, "STOPPINGBUSSERVICE"},
    {704, "LOCALBUSSERVICE"},
    {705, "NIGHTBUSSERVICE"},
    {706, "POSTBUSSERVICE"},
    {707, "SPECIALNEEDSBUS"},
    {708, "MOBILITYBUSSERVICE"},
    {709, "MOBILITYBUSFORREGISTEREDDISABLED"},
    {710, "SIGHTSEEINGBUS"},
    {711, "SHUTTLEBUS"},
    {712, "SCHOOLBUS"},
    {713, "SCHOOLANDPUBLICSERVICEBUS"},
    {714, "RAILREPLACEMENTBUSSERVICE"},
    {715, "DEMANDANDRESPONSEBUSSERVICE"},
    {716, "ALLBUSSERVICES"},
    {800, "TROLLEYBUSSERVICE"},
    {900, "TRAMSERVICE"},
    {901, "CITYTRAMSERVICE"},
    {902, "LOCALTRAMSERVICE"},
    {903, "REGIONALTRAMSERVICE"},
    {904, "SIGHTSEEINGTRAMSERVICE"},
    {905, "SHUTTLETRAMSERVICE"},
    {906, "ALLTRAMSERVICES"},
    {1000, "WATERTRANSPORTSERVICE"},
    {1100, "AIRSERVICE"},
    {1200, "FERRYSERVICE"},
    {1300, "AERIALLIFTSERVICE"},
    {1301, "TELECABINSERVICE"},
    {1302, "CABLECARSERVICE"},
    {1303, "ELEVATORSERVICE"},
    {1304, "CHAIRLIFTSERVICE"},
    {1305, "DRAGLIFTSERVICE"},
    {1306, "SMALLTELECABINSERVICE"},
    {1307, "ALLTELECABINSERVICES"},
    {1400, "FUNICULARSERVICE"},
    {1500, "TAXISERVICE"},
    {1501, "COMMUNALTAXISERVICE"},
    {1502, "WATERTAXISERVICE"},
    {1503, "RAILTAXISERVICE"},
    {1504, "BIKETAXISERVICE"},
    {1505, "LICENSEDTAXISERVICE"},
    {1506, "PRIVATEHIRESERVICEVEHICLE"},
    {1507, "ALLTAXISERVICES"},
    {1700, "MISCELLANEOUSSERVICE"},
    {1702, "HORSE-DRAWNCARRIAGE"}
};

QTextStream& operator<<(QTextStream& os, const Journey &journey) {
    os << "Journey from " << journey.source->name << " to " << journey.target->name << "\n";
    os << journey.startTime << " - " << journey.arrivalTime << "\n";
    if (journey.empty()) {
        os << "Source and target stops are the same." << "\n";
    }
    for (const auto &journeyElement : journey) {
        if (std::holds_alternative<Ride>(journeyElement)) {
            auto &ride = std::get<Ride>(journeyElement);
            os << "\n";
            os << routeTypeNames[ride.route->type] << " " << ride.route->name << "\n";
            os << (*ride.firstStop)->name << " -> " << (*ride.lastStop)->name << "\n";
            os << ride.startTime() << " - " << ride.endTime() << "\n";
        } else {
            auto &transfer = std::get<Transfer>(journeyElement);
            os << "\n";
            os << "TRANSFER from " << transfer.start->name << " to " << transfer.end->name
                    << " (" << transfer.time / 60 << " min)" << "\n";
        }
    }
    os << "\n\n";

    return os;
}

}
