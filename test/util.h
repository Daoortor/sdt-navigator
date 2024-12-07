#ifndef UTIL_H
#define UTIL_H

#include "../models/include/transportSystem.h"
#include "../models/include/util.h"

#include <string>
#include <vector>

namespace sdtmaps::test {

enum class JourneyElementType {
    RIDE,
    TRANSFER
};

struct SimplifiedJourneyElement {
    JourneyElementType type{};
    QString startId;
    QString endId;

    bool operator==(const SimplifiedJourneyElement &other) const = default;
};

// Simplified journey structure for testing
struct SimplifiedJourney {
    std::vector<SimplifiedJourneyElement> elements;
    DateTime arrivalTime{};

    SimplifiedJourney() = default;
    explicit SimplifiedJourney(const Journey &journey);
    SimplifiedJourney(const std::vector<SimplifiedJourneyElement> &elements, const DateTime &arrivalTime): elements(elements), arrivalTime(arrivalTime) {};

    bool operator==(const SimplifiedJourney &other) const = default;
};

struct TestCase {
    TransportSystem &transportSystem;
    QString startId;
    QString endId;
    DateTime startTime;
    SimplifiedJourney &&expected;

    void check() const;
};

}

#endif //UTIL_H
