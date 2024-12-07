#include "util.h"

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
            thisIt->startId = (*ride.firstStop)->id.toStdString();
            thisIt->endId = (*ride.lastStop)->id.toStdString();
            currentTime = ride.endTime();
        } else {
            thisIt->type = JourneyElementType::TRANSFER;
            Transfer transfer = std::get<Transfer>(*otherIt);
            thisIt->startId = transfer.start->id.toStdString();
            thisIt->endId = transfer.end->id.toStdString();
            currentTime += transfer.time;
        }
    }
    arrivalTime = currentTime;
}

void TestCase::check() const {
    auto j = pathfind(transportSystem, startId, endId, startTime);
    assert(j.has_value());
    SimplifiedJourney actual = SimplifiedJourney(j.value());
    assert(expected == actual);
}

}
