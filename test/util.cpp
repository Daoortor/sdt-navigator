#include "util.h"

#include <iostream>

#include "../core/include/pathfinder.h"

namespace sdtmaps::test {

SimplifiedJourney::SimplifiedJourney(const Journey &journey) {
    if (journey.empty()) {
        return;
    }
    elements.resize(journey.size());
    auto thisIt = elements.begin();
    auto otherIt = journey.begin();
    DateTime currentTime{};
    for (; otherIt != journey.end(); ++otherIt, ++thisIt) {
        if (std::holds_alternative<Ride>(*otherIt)) {
            thisIt->type = JourneyElementType::RIDE;
            Ride ride = std::get<Ride>(*otherIt);
            thisIt->startId = (*ride.firstStop)->id;
            thisIt->endId = (*ride.lastStop)->id;
            currentTime = ride.endTime();
        } else {
            thisIt->type = JourneyElementType::TRANSFER;
            Transfer transfer = std::get<Transfer>(*otherIt);
            thisIt->startId = transfer.start->id;
            thisIt->endId = transfer.end->id;
            currentTime += transfer.time;
        }
    }
    arrivalTime = currentTime;
}

QTextStream &operator<<(QTextStream &os, const SimplifiedJourney &journey) {
    os << "\n\n";
    for (auto &element : journey.elements) {
        os << element.type._to_string() << " " << element.startId << " -> " << element.endId << "\n";
    }
    os << "Arrival time: " << journey.arrivalTime << "\n";
    return os;
}


void TestCase::check() const {
    auto j = pathfind(transportSystem, startId, endId, startTime);
    assert(j.has_value());
    SimplifiedJourney actual = SimplifiedJourney(j.value());
    QTextStream cout(stdout, QIODevice::WriteOnly);
    cout << j.value();
    if (expected != actual) {
        cout << "expected " << expected << " but got " << actual;
        Qt::endl(cout);
        assert(expected == actual);
    }
}

QString generateString(size_t len) {
    std::string result;
    result.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        result += 'a' + randomChar(rng);
    }

    return result.c_str();
}


std::vector<QString> generateStrings(size_t n, size_t len) {
    std::vector<QString> result;
    result.reserve(n);
    for (size_t i = 0; i < n; i++) {
        result.push_back(generateString(len));
    }
    return result;
}

std::vector<QString> naiveFindAllStringsContaining(const std::vector<QString> &strings, const QString &substring) {
    std::vector<QString> result;
    std::string stdSubstring = substring.toStdString();
    for (auto &string : strings) {
        if (string.toStdString().find(stdSubstring) != string.toStdString().npos) {
            result.push_back(string);
        }
    }
    std::ranges::sort(result);
    return result;
}

}
