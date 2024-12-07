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

}
