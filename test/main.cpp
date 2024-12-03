#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../lib/doctest.h"

#include "test-data/minsk-subway.h"
#include "../core/include/pathfinder.h"

namespace sdtmaps::tests {

TEST_CASE("Minsk subway") {
    TransportSystem minskSubway = makeMinskSubway();
    SUBCASE("isValid") {
        assert(minskSubway.isValid());
    }

    // TODO(Uncomment this after implementing pathfind())
    SUBCASE("Trivial journey") {
        std::optional<Journey> j_ = pathfind(minskSubway, "113", "113", QTime(3, 30));
        assert(j_.has_value());
        Journey &j = j_.value();
        assert(j.empty());
    }

    SUBCASE("Simple journey") {
        std::optional<Journey> j_ = pathfind(minskSubway, "113", "114", QTime(3, 30));
        assert(j_.has_value());
        Journey &j = j_.value();
        assert(j.size() == 1);
        assert(std::holds_alternative<Ride>(j[0]));
    }

    SUBCASE("One change") {
        std::optional<Journey> j_ = pathfind(minskSubway, "214", "119", QTime(4, 00));
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

}
