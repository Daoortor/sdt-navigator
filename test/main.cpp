#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "test-data/minsk-subway.h"
#include "../core/include/pathfinder.h"
#include "../models/include/util.h"
#include "util.h"

namespace sdtmaps::test {

TEST_CASE("Minsk subway") {
    TransportSystem minskSubway = makeMinskSubway();

    REQUIRE(minskSubway.isValid());

    SUBCASE("Trivial journey") {
        TestCase(minskSubway, "113", "113", QDateTime(minskSubwayDate, QTime(3, 30)), {}).check();
    }

    SUBCASE("Simple journey") {
        TestCase(
            minskSubway, "113", "114", QDateTime(minskSubwayDate, QTime(3, 30)),
            {
                {
                    {JourneyElementType::RIDE, "113", "114"}
                },
                QDateTime(minskSubwayDate, QTime(4, 20))
        }).check();
    }

    SUBCASE("One change") {
        TestCase(
            minskSubway, "214", "119", QDateTime(minskSubwayDate, QTime(3, 30)),
            {
                {
                    {JourneyElementType::RIDE, "214", "216"},
                    {JourneyElementType::TRANSFER, "216", "116"},
                    {JourneyElementType::RIDE, "116", "119"}
                },
                QDateTime(minskSubwayDate, QTime(4, 45))
            }
        );
    }
}

TEST_CASE("Hamburg") {
    TransportSystem hamburg(QDir(PROJECT_ROOT_PATH "/data/gtfs_hamburg"));
    QDate hamburgDate(2020, 03, 20);

    REQUIRE(hamburg.isValid());

    SUBCASE("Iserbrook - Hamburg Hbf") {
        TestCase(
            hamburg, "000000081951", "000000010950", QDateTime(hamburgDate, QTime(6, 00)),
            {
                {
                    {JourneyElementType::RIDE, "000000081951", "000000080953"},
                    {JourneyElementType::RIDE, "000000080953", "000000010950"}
                },
                QDateTime(hamburgDate, QTime(6, 31))
            }
        ).check();
    }

    SUBCASE("Bremen Hbf - Kiel Hbf") {
        TestCase(
            hamburg, "000008000050_G", "000008000199_G", QDateTime(hamburgDate, QTime(6, 00)),
            {
                {
                    {JourneyElementType::RIDE, "000008000050_G", "000008000147_G"},
                    {JourneyElementType::RIDE, "000008000147_G", "000000010950"},
                    {JourneyElementType::RIDE, "000000010950", "000008000199_G"},
                },
                QDateTime(hamburgDate, QTime(9, 32))
            }
        ).check();
    }
}

TEST_CASE("Paris") {
    TransportSystem paris(QDir(PROJECT_ROOT_PATH "/data/gtfs_paris"));
    QDate parisDate(2021, 03, 25);

    REQUIRE(paris.isValid());

    SUBCASE("Temple - Pasteur") {
        TestCase(
            paris, "1683", "1781", QDateTime(parisDate, QTime(6, 00)),
            {
                {
                    {JourneyElementType::RIDE, "1683", "1889"},
                    {JourneyElementType::TRANSFER, "1889", "2053"},
                    {JourneyElementType::RIDE, "2053", "1967"},
                    {JourneyElementType::RIDE, "1967", "1996"},
                    {JourneyElementType::RIDE, "1996", "1781"},
                },
                QDateTime(parisDate, QTime(8, 39))
            }
        ).check();
    }
}

}
