#include <iostream>
#include <QtCore>
#include <QCommandLineParser>

#include "../common/common.h"
#include "../core/include/pathfinder.h"
#include "../models/include/transportSystem.h"
#include "../lib/CLI11/include/CLI/CLI.hpp"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cin(stdin, QIODevice::ReadOnly);

const QString PROGRAM_NAME = "sdt-navigator";
const QString CLI_HELP_MESSAGE = R"(
*********************************SDT-NAVIGATOR**********************************

COMMANDS
    search <substring>

        Outputs all the stops in the dataset whose names contain <substring>. Stops are ordered by the number of routes passing through them.

    route --start <start-identifier> --end <end-identifier> [--date <date>] [--time <time>]

        Outputs the fastest (by arrival time) journey in the dataset from <start-identifier> to <end-identifier>, departing on <date> at <time>.

        Arguments:
            start-identifier         ID or full name of the start station.
            end-identifier           ID or full name of the end station.
            date                     Departure date in yyyy-mm-dd format. Defaults to 1970-01-01.
            time                     Departure time in h:mm format. Defaults to 0:00.


    help

        Outputs this message.


    exit, quit

        Exits the interpreter.

********************************************************************************
)";

void handleHelpCommand() {
    cout << CLI_HELP_MESSAGE << Qt::flush;
}

void handleRouteCommand(const sdtmaps::TransportSystem &transportSystem, std::vector<std::string> &args) {
    std::string startArg;
    std::string endArg;
    std::string dateArg;
    std::string timeArg;
    CLI::App app{"route"};
    app.add_option("--start", startArg, "ID or full name of the start station")->required();
    app.add_option("--end", endArg, "ID or full name of the end station")->required();
    app.add_option("--date", dateArg, "departure date in yyyy-MM-dd format; defaults to the first date in the schedule");
    app.add_option("--time", timeArg, "departure time in HH:mm format; defaults to 00:00");
    try {
        app.parse(std::vector(args.begin(), args.end() - 1));
    } catch (const CLI::ParseError &e) {
        std::cerr << e.what() << std::endl;
        return;
    }
    startArg = CLI::detail::remove_quotes(startArg);
    endArg = CLI::detail::remove_quotes(endArg);
    dateArg = CLI::detail::remove_quotes(dateArg);
    timeArg = CLI::detail::remove_quotes(timeArg);

    const sdtmaps::Stop *start = transportSystem.getStopById(startArg.c_str());
    if (!start) {
        start = transportSystem.getStopByName(startArg.c_str());
    }
    if (!start) {
        std::vector <const sdtmaps::Stop *> searchResult = transportSystem.getStopsBySubstring(startArg.c_str());
        if (searchResult.empty()) {
            qDebug() << "Start station '" << startArg.c_str() << "' not found\n" << Qt::flush;
            return;
        }
        start = searchResult[0];
    }
    const sdtmaps::Stop *end = transportSystem.getStopById(endArg.c_str());
    if (!end) {
        end = transportSystem.getStopByName(endArg.c_str());
    }
    if (!end) {
        std::vector <const sdtmaps::Stop *> searchResult = transportSystem.getStopsBySubstring(endArg.c_str());
        if (searchResult.empty()) {
            qDebug() << "Destination station '" << endArg.c_str() << "' not found\n" << Qt::flush;
            return;
        }
        end = searchResult[0];
    }

    QDate initDate = dateArg.empty() ? QDate::fromJulianDay(0) : QDate::fromString(dateArg.c_str(), "yyyy-MM-dd");
    if (!initDate.isValid()) {
        qDebug() << "Invalid date format: " << dateArg.c_str() << ". Should be yyyy-MM-dd." << Qt::flush;
        return;
    }
    QTime initTime = timeArg.empty() ? QTime(0, 0) : QTime::fromString(timeArg.c_str());
    if (!initTime.isValid()) {
        qDebug() << "Invalid time format: " << timeArg.c_str() << ". Should be HH:mm." << Qt::flush;
        return;
    }
    QDateTime initDateTime = QDateTime(initDate, initTime);
    std::optional<sdtmaps::Journey> result = pathfind(transportSystem, start->id, end->id, initDateTime);
    if (!result.has_value()) {
        qDebug() << "No journey found.\n" << Qt::flush;
        return;
    }
    sdtmaps::Journey &journey = result.value();
    cout << journey << Qt::flush;
}

void handleSearchCommand(const sdtmaps::TransportSystem &transportSystem, std::vector<std::string> &args) {
    std::string substringArg;
    CLI::App app{"search"};
    app.add_option("substring", substringArg, "Substring to be searched among stops")->required();
    try {
        app.parse(std::vector(args.begin(), args.end() - 1));
    } catch (const CLI::ParseError &e) {
        std::cerr << e.what() << std::endl;
        return;
    }
    substringArg = CLI::detail::remove_quotes(substringArg);

    std::vector<const sdtmaps::Stop *> result = transportSystem.getStopsBySubstring(substringArg.c_str());
    if (result.empty()) {
        qDebug() << "No stops found.\n" << Qt::flush;
        return;
    }
    cout << "\nFound " << result.size() << " stops.\n\n";
    size_t maxNameLength = 4;
    for (const sdtmaps::Stop *stop : result) {
        if (maxNameLength < stop->name.size()) {
            maxNameLength = stop->name.size();
        }
    }
    size_t maxIdLength = 0;
    for (const sdtmaps::Stop *stop : result) {
        if (maxIdLength < stop->id.size()) {
            maxIdLength = stop->id.size();
        }
    }
    cout << "| ID" << std::string(maxIdLength, ' ').c_str() << " | NAME" << std::string(maxNameLength - 4, ' ').c_str() << " | ROUTE_COUNT |\n";
    for (const sdtmaps::Stop *stop : result) {
        cout << "| " << stop->id << std::string(maxIdLength - stop->id.length() + 2, ' ').c_str() << " | " <<
            stop->name << std::string(maxNameLength - stop->name.length(), ' ').c_str() << " | " <<
            stop->routeCount << std::string(11 - std::to_string(stop->routeCount).size(), ' ').c_str() << " |\n";
    }
    cout << "\n\n" << Qt::flush;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(PROGRAM_NAME);
    QCoreApplication::setApplicationVersion("1.0");
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addPositionalArgument("dataset",
        "The name of the dataset to load transport data from. Datasets are stored in the " PROJECT_DATA_ROOT_RELATIVE_PATH " directory. Example: 'gtfs_hamburg'.");
    parser.addHelpOption();
    parser.process(app);

    const QStringList positionalArgs = parser.positionalArguments();
    if (positionalArgs.size() != 1) {
        std::cerr << "Expected 1 positional argument(s): dataset, but got " << positionalArgs.size() << std::endl;
        std::cerr << "Try ./sdt-navigator help for help." << std::endl;
        return EXIT_FAILURE;
    }
    const QString &dataset = positionalArgs[0];

    sdtmaps::TransportSystem transportSystem;

    try {
        transportSystem = sdtmaps::TransportSystem(QDir(QString(PROJECT_DATA_ROOT "/") + dataset));
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Try ./sdt-navigator help for help." << std::endl;
        return EXIT_FAILURE;
    }

    std::string input;
    cout << "\n[" + PROGRAM_NAME + "]$ " << Qt::flush;
    while (std::getline(std::cin, input)) {
        std::vector<std::string> args = CLI::detail::split_up(input);
        if (args.empty()) {
            continue;
        }
        std::string commandName = args[0];
        std::ranges::reverse(args);
        if (commandName == "route") {
            handleRouteCommand(transportSystem, args);
        } else if (commandName == "search") {
            handleSearchCommand(transportSystem, args);
        } else if (commandName == "quit" || commandName == "exit") {
            break;
        } else if (commandName == "help") {
            handleHelpCommand();
        } else {
            qDebug() << "Unknown command: " << commandName.c_str() << ". Type help for help." << Qt::flush;
        }
        cout << "[" + PROGRAM_NAME + "]$ " << Qt::flush;
    }

    return EXIT_SUCCESS;
}
