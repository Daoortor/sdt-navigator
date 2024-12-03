#include "minsk-subway.h"

namespace sdtmaps {

/*
 * Partial model of Minsk subway system.
 * https://www.belarus.by/dadvimages/s001321_161902.jpg
 */
TransportSystem makeMinskSubway() {
    auto subway = TransportSystem();
    subway.stops.resize(21);
    subway.stopRoutes.resize(42);
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
        {"Malinauka", "110", &*subway.transfers.end(), &subway.stopRoutes[0], 2}, // 0
        {"Piatrouscyna", "111", &*subway.transfers.end(), &subway.stopRoutes[2], 2},
        {"Nichalova", "112", &*subway.transfers.end(), &subway.stopRoutes[4], 2},
        {"Hrushauka", "113", &*subway.transfers.end(), &subway.stopRoutes[6], 2},
        {"Instytut kultury", "114", &*subway.transfers.end(), &subway.stopRoutes[8], 2},
        {"Ploshcha Lenina", "115", &subway.transfers[0], &subway.stopRoutes[10], 2}, // 5
        {"Kastrychnickaya", "116", &subway.transfers[1], &subway.stopRoutes[12], 2},
        {"Ploshcha Peramogi", "117", &*subway.transfers.end(), &subway.stopRoutes[14], 2},
        {"Ploshcha Jakuba Kopasa", "118", &*subway.transfers.end(), &subway.stopRoutes[16], 2},
        {"Akademija navuk", "119", &*subway.transfers.end(), &subway.stopRoutes[18], 2},
        {"Pralietarskaya", "214", &*subway.transfers.end(), &subway.stopRoutes[20], 2}, // 10
        {"Piershamayskaya", "215", &*subway.transfers.end(), &subway.stopRoutes[22], 2},
        {"Kupalayskaya", "216", &subway.transfers[2], &subway.stopRoutes[24], 2},
        {"Niamiha", "217", &*subway.transfers.end(), &subway.stopRoutes[26], 2},
        {"Frunzenskaya", "218", &subway.transfers[3], &subway.stopRoutes[28], 2},
        {"Maladzjozhnaya", "219", &*subway.transfers.end(), &subway.stopRoutes[30], 2}, // 15
        {"Pushkinskaya", "220", &*subway.transfers.end(), &subway.stopRoutes[32], 2},
        {"Kavalskaya slabada", "313", &*subway.transfers.end(), &subway.stopRoutes[34], 2},
        {"Vakzalnaya", "314", &subway.transfers[4], &subway.stopRoutes[36], 2},
        {"Ploshcha Franciska Bagushevicha", "315", &*subway.transfers.end(), &subway.stopRoutes[38], 2},
        {"Yubileynaya Ploshcha", "316", &subway.transfers[5], &subway.stopRoutes[40], 2}, // 20
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
        {&subway.routes[0], &subway.routeStops[0]}, {&subway.routes[1], &subway.routeStops[19]},
        {&subway.routes[0], &subway.routeStops[1]}, {&subway.routes[1], &subway.routeStops[18]},
        {&subway.routes[0], &subway.routeStops[2]}, {&subway.routes[1], &subway.routeStops[17]},
        {&subway.routes[0], &subway.routeStops[3]}, {&subway.routes[1], &subway.routeStops[16]},
        {&subway.routes[0], &subway.routeStops[4]}, {&subway.routes[1], &subway.routeStops[15]},
        {&subway.routes[0], &subway.routeStops[5]}, {&subway.routes[1], &subway.routeStops[14]},
        {&subway.routes[0], &subway.routeStops[6]}, {&subway.routes[1], &subway.routeStops[13]},
        {&subway.routes[0], &subway.routeStops[7]}, {&subway.routes[1], &subway.routeStops[12]},
        {&subway.routes[0], &subway.routeStops[8]}, {&subway.routes[1], &subway.routeStops[11]},
        {&subway.routes[0], &subway.routeStops[9]}, {&subway.routes[1], &subway.routeStops[10]},
        {&subway.routes[2], &subway.routeStops[20]}, {&subway.routes[3], &subway.routeStops[33]},
        {&subway.routes[2], &subway.routeStops[21]}, {&subway.routes[3], &subway.routeStops[32]},
        {&subway.routes[2], &subway.routeStops[22]}, {&subway.routes[3], &subway.routeStops[31]},
        {&subway.routes[2], &subway.routeStops[23]}, {&subway.routes[3], &subway.routeStops[30]},
        {&subway.routes[2], &subway.routeStops[24]}, {&subway.routes[3], &subway.routeStops[29]},
        {&subway.routes[2], &subway.routeStops[25]}, {&subway.routes[3], &subway.routeStops[28]},
        {&subway.routes[2], &subway.routeStops[26]}, {&subway.routes[3], &subway.routeStops[27]},
        {&subway.routes[4], &subway.routeStops[34]}, {&subway.routes[5], &subway.routeStops[41]},
        {&subway.routes[4], &subway.routeStops[35]}, {&subway.routes[5], &subway.routeStops[40]},
        {&subway.routes[4], &subway.routeStops[36]}, {&subway.routes[5], &subway.routeStops[39]},
        {&subway.routes[4], &subway.routeStops[37]}, {&subway.routes[5], &subway.routeStops[38]},
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
