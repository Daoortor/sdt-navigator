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
    route --start <start-identifier> --end <end-identifier> [--date <date>] [--time <time>]

        Outputs the fastest (by arrival time) journey in the given dataset from <start-identifier> to <end-identifier>, departing on <date> at <time>.

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
    app.add_option("--date", dateArg, "departure date in yyyy-mm-dd format; defaults to the first date in the schedule");
    app.add_option("--time", timeArg, "departure time in h:mm format; defaults to 0:00");
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
        qDebug() << "Start station '" << startArg.c_str() << "' not found\n";
        return;
    }
    const sdtmaps::Stop *end = transportSystem.getStopById(endArg.c_str());
    if (!end) {
        end = transportSystem.getStopByName(endArg.c_str());
    }
    if (!end) {
        qDebug() << "End station '" << endArg.c_str() << "' not found\n";
        return;
    }
    QDate initDate = dateArg.empty() ? QDate::fromJulianDay(0) : QDate::fromString(dateArg.c_str(), "yyyy-mm-dd");
    QTime initTime = timeArg.empty() ? QTime(0, 0) : QTime::fromString(dateArg.c_str(), "h:mm");
    std::optional<sdtmaps::Journey> result = pathfind(transportSystem, start->id, end->id, QDateTime(initDate, initTime));
    if (!result.has_value()) {
        qDebug() << "No journey found\n";
        return;
    }
    sdtmaps::Journey &journey = result.value();
    cout << journey << Qt::flush;
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
        return EXIT_FAILURE;
    }
    const QString &dataset = positionalArgs[0];

    sdtmaps::TransportSystem transportSystem;

    try {
        transportSystem = sdtmaps::TransportSystem(QDir(QString(PROJECT_DATA_ROOT "/") + dataset));
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
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
        } else if (commandName == "help") {
            handleHelpCommand();
        } else if (commandName == "quit" || commandName == "exit") {
            break;
        } else {
            qDebug() << "Unknown command: " << commandName.c_str();
        }
        cout << "[" + PROGRAM_NAME + "]$ " << Qt::flush;
    }

    return EXIT_SUCCESS;
}
