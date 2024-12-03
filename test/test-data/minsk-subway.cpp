#include "minsk-subway.h"

namespace sdtmaps {

/*
 * Partial model of Minsk subway system.
 * https://www.belarus.by/dadvimages/s001321_161902.jpg
 */
TransportSystem makeMinskSubway() {
    auto subway = TransportSystem();
    subway.stops.resize(21);
    subway.stopRoutes.resize(54);
    subway.stopTimes.resize(966);
    subway.routeStops.resize(42);

    subway.transfers = {
        {&subway.stops[5], &subway.stops[18], 300},
        {&subway.stops[6], &subway.stops[12], 300},
        {&subway.stops[12], &subway.stops[6], 300},
        {&subway.stops[14], &subway.stops[20], 300},
        {&subway.stops[18], &subway.stops[5], 300},
        {&subway.stops[20], &subway.stops[14], 300},
    };
    subway.stops = {
        {"Malinauka", "110", &*subway.transfers.end(), &subway.stopRoutes[0]}, // 0
        {"Piatrouscyna", "111", &*subway.transfers.end(), &subway.stopRoutes[2]},
        {"Nichalova", "112", &*subway.transfers.end(), &subway.stopRoutes[4]},
        {"Hrushauka", "113", &*subway.transfers.end(), &subway.stopRoutes[6]},
        {"Instytut kultury", "114", &*subway.transfers.end(), &subway.stopRoutes[8]},
        {"Ploshcha Lenina", "115", &subway.transfers[0], &subway.stopRoutes[10]}, // 5
        {"Kastrychnickaya", "116", &subway.transfers[1], &subway.stopRoutes[14]},
        {"Ploshcha Peramogi", "117", &*subway.transfers.end(), &subway.stopRoutes[18]},
        {"Ploshcha Jakuba Kopasa", "118", &*subway.transfers.end(), &subway.stopRoutes[20]},
        {"Akademija navuk", "119", &*subway.transfers.end(), &subway.stopRoutes[22]},
        {"Pralietarskaya", "214", &*subway.transfers.end(), &subway.stopRoutes[24]}, // 10
        {"Piershamayskaya", "215", &*subway.transfers.end(), &subway.stopRoutes[26]},
        {"Kupalayskaya", "216", &subway.transfers[2], &subway.stopRoutes[28]},
        {"Niamiha", "217", &*subway.transfers.end(), &subway.stopRoutes[32]},
        {"Frunzenskaya", "218", &subway.transfers[3], &subway.stopRoutes[34]},
        {"Maladzjozhnaya", "219", &*subway.transfers.end(), &subway.stopRoutes[38]}, // 15
        {"Pushkinskaya", "220", &*subway.transfers.end(), &subway.stopRoutes[40]},
        {"Kavalskaya slabada", "313", &*subway.transfers.end(), &subway.stopRoutes[42]},
        {"Vakzalnaya", "314", &subway.transfers[4], &subway.stopRoutes[44]},
        {"Ploshcha Franciska Bagushevicha", "315", &*subway.transfers.end(), &subway.stopRoutes[48]},
        {"Yubileynaya Ploshcha", "316", &subway.transfers[5], &subway.stopRoutes[50]}, // 20
    };
    subway.routes = {
        {23, 10, &subway.stopTimes[0], &subway.routeStops[0]},
        {23, 10, &subway.stopTimes[230], &subway.routeStops[10]},
        {23, 7, &subway.stopTimes[460], &subway.routeStops[20]},
        {23, 7, &subway.stopTimes[621], &subway.routeStops[27]},
        {23, 4, &subway.stopTimes[782], &subway.routeStops[34]},
        {23, 4, &subway.stopTimes[874], &subway.routeStops[38]},
    };
    subway.stopRoutes = {
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1], &subway.routes[4], &subway.routes[5],
        &subway.routes[0], &subway.routes[1], &subway.routes[2], &subway.routes[3],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[0], &subway.routes[1],
        &subway.routes[2], &subway.routes[3],
        &subway.routes[2], &subway.routes[3],
        &subway.routes[2], &subway.routes[3], &subway.routes[0], &subway.routes[1],
        &subway.routes[2], &subway.routes[3],
        &subway.routes[2], &subway.routes[3], &subway.routes[4], &subway.routes[5],
        &subway.routes[2], &subway.routes[3],
        &subway.routes[2], &subway.routes[3],
        &subway.routes[4], &subway.routes[5],
        &subway.routes[4], &subway.routes[5], &subway.routes[0], &subway.routes[1],
        &subway.routes[4], &subway.routes[5],
        &subway.routes[4], &subway.routes[5], &subway.routes[2], &subway.routes[3],
    };
    subway.routeStops = {
        &subway.stops[0], &subway.stops[1], &subway.stops[2], &subway.stops[3], &subway.stops[4], &subway.stops[5], &subway.stops[6], &subway.stops[7], &subway.stops[8], &subway.stops[9],
        &subway.stops[9], &subway.stops[8], &subway.stops[7], &subway.stops[6], &subway.stops[5], &subway.stops[4], &subway.stops[3], &subway.stops[2], &subway.stops[1], &subway.stops[0],
        &subway.stops[10], &subway.stops[11], &subway.stops[12], &subway.stops[13], &subway.stops[14], &subway.stops[15], &subway.stops[16],
        &subway.stops[16], &subway.stops[15], &subway.stops[14], &subway.stops[13], &subway.stops[12], &subway.stops[11], &subway.stops[10],
        &subway.stops[17], &subway.stops[18], &subway.stops[19], &subway.stops[20],
        &subway.stops[20], &subway.stops[19], &subway.stops[18], &subway.stops[17],
    };
    for (const auto &route : subway.routes) {
        for (int tripIndex = 0; tripIndex < route.tripCount; tripIndex++) {
            for (int stopIndex = 0; stopIndex < route.stopCount; stopIndex++) {
                subway.stopTimes[tripIndex * route.stopCount + stopIndex] = {
                    QTime(tripIndex, 5 * stopIndex), QTime(tripIndex, 5 * stopIndex + 1)
                };
            }
        }
    }

    return subway;
}

}
