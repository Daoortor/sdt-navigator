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

    // TODO(Uncomment this after implementing pathfind())
    // SUBCASE("Trivial journey") {
    //     Journey j = pathfind(minskSubway, "113", "113", QTime(3, 30));
    //     assert(j.empty());
    // }
    //
    // SUBCASE("Simple journey") {
    //     Journey j = pathfind(minskSubway, "113", "114", QTime(3, 30));
    //     assert(j.size() == 1);
    //     assert(std::holds_alternative<Ride>(j[0]));
    // }
    //
    // SUBCASE("One change") {
    //     Journey j = pathfind(minskSubway, "214", "119", QTime(4, 00));
    //     assert(j.size() == 3);
    //     assert(std::holds_alternative<Ride>(j[0]));
    //     assert(std::holds_alternative<Transfer>(j[1]));
    //     assert(std::holds_alternative<Ride>(j[2]));
    // }
}

}
