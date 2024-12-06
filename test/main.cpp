#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "test-data/minsk-subway.h"
#include "../core/include/pathfinder.h"

namespace sdtmaps::tests {

TEST_CASE("Minsk subway") {
    TransportSystem minskSubway = makeMinskSubway();
    SUBCASE("isValid") {
        assert(minskSubway.isValid());
    }

    SUBCASE("Trivial journey") {
        std::optional<Journey> j_ = pathfind(minskSubway, "113", "113",
            QDateTime(minskSubwayDate, QTime(3, 30)));
        assert(j_.has_value());
        Journey &j = j_.value();
        assert(j.empty());
    }

    SUBCASE("Simple journey") {
        QString startId = "113";
        QString endId = "114";
        QDateTime startDateTime = QDateTime(minskSubwayDate, QTime(3, 30));
        std::optional<Journey> j_ = pathfind(minskSubway, startId, endId, startDateTime);
        assert(j_.has_value());
        Journey &j = j_.value();
        assert(j.size() == 1);
        assert(std::holds_alternative<Ride>(j[0]));
        assert((*std::get<Ride>(j[0]).firstStop)->id == startId);
        assert((*std::get<Ride>(j[0]).lastStop)->id == endId);
        assert(std::get<Ride>(j[0]).firstStopTime->arrivalTime == QDateTime(minskSubwayDate, QTime(4, 00)));
    }

    SUBCASE("One change") {
        std::optional<Journey> j_ = pathfind(minskSubway, "214", "119",
            QDateTime(minskSubwayDate, QTime(4, 00)));
        assert(j_.has_value());
        Journey &j = j_.value();
        assert(j.size() == 3);
        assert(std::holds_alternative<Ride>(j[0]));
        assert(std::holds_alternative<Transfer>(j[1]));
        assert(std::holds_alternative<Ride>(j[2]));
    }
}

TEST_CASE("Hamburg") {
    TransportSystem hamburg(QDir(PROJECT_ROOT_PATH "/data/gtfs_hamburg"));
    SUBCASE("isValid") {
        assert(hamburg.isValid());
    }
}

TEST_CASE("Paris") {
    TransportSystem paris(QDir(PROJECT_ROOT_PATH "/data/gtfs_paris"));
    SUBCASE("isValid") {
        assert(paris.isValid());
    }
}

}
