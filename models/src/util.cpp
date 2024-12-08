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

std::ostream& operator<<(std::ostream& os, const DateTime &dt) {
    QDateTime QTdt = QDateTime::fromSecsSinceEpoch(dt.timestamp);
    return os << QTdt.toString().toStdString();
}

DateTime Ride::startTime() const {
    return firstStopTime->departureTime;
}

DateTime Ride::endTime() const {
    return (firstStopTime + (lastStop - firstStop))->arrivalTime;
}

std::ostream& operator<<(std::ostream& os, const Journey &journey) {
    os << "Journey from " << journey.source->name.toStdString() << " to " << journey.target->name.toStdString() << std::endl;
    os << journey.startTime << " - " << journey.arrivalTime << std::endl;
    if (journey.empty()) {
        os << "Source and target stops are the same." << std::endl;
    }
    for (const auto &journeyElement : journey) {
        if (std::holds_alternative<Ride>(journeyElement)) {
            auto &ride = std::get<Ride>(journeyElement);
            os << std::endl;
            os << ride.route->type._to_string() << " " << ride.route->name.toStdString() << std::endl;
            os << (*ride.firstStop)->name.toStdString() << " -> " << (*ride.lastStop)->name.toStdString() << std::endl;
            os << ride.startTime() << " - " << ride.endTime() << std::endl;
        } else {
            auto &transfer = std::get<Transfer>(journeyElement);
            os << std::endl;
            os << "TRANSFER from " << transfer.start->name.toStdString() << " to " << transfer.end->name.toStdString()
                    << " (" << transfer.time / 60 << " min)" << std::endl;
        }
    }
    os << std::endl << std::endl;

    return os;
}

}
