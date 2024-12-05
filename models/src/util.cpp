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

DateTime DateTime::addSecs(int secs) const {
    return DateTime(this->timestamp + secs);
}

}
