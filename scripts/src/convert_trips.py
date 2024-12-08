from datetime import datetime, date, time
from collections import defaultdict
import os

import simdjson as json
import pandas as pd

from src.ts_types import Station, StopTime, Stop, Trip, Route
from src.utils import mkpath, dump_json
from src.grammar import prettify_names


ROUTES_COLUMNS = {
    'route_id': str,
    'route_short_name': str,
    'route_long_name': str,
    'route_type': 'int8'
}
TRIPS_COLUMNS = {
    'route_id': str,
    'trip_id': int
}
STOP_TIMES_COLUMNS = {
    'trip_id': int,
    'stop_id': str,
    'arrival_time': str,
    'departure_time': str,
    'stop_sequence': 'int8'
}


def convert_trips(raw_data_path: str, data_path: str, data_date: date):
    DEBUG = (os.environ.get('_CONVERTER_DEBUG') == '1')
    data_timestamp = int(datetime.combine(data_date, time(0, 0)).timestamp())
    print('Converting trips...')

    assert os.path.isfile(mkpath(data_path, 'stations.json')), 'Run convert_stations.py first'
    assert os.path.isfile(mkpath(data_path, 'stations_map.json')), 'Run convert_stations.py first'

    with open(mkpath(data_path, 'stations.json'), 'r', encoding='utf-8') as file:
        stations_by_id = {station.id: station for station in map(Station._make, json.load(file))}

    with open(mkpath(data_path, 'stations_map.json'), 'r', encoding='utf-8') as file:
        stations_by_id.update(
            (map_from, stations_by_id[map_to])
            for map_from, map_to in json.load(file).items()
        )

    print('Reading trips.csv...')
    trips_csv = pd.read_csv(
        mkpath(raw_data_path, 'trips.csv'),
        usecols=tuple(TRIPS_COLUMNS.keys()),
        dtype=TRIPS_COLUMNS,
        na_filter=False
    )

    print(f'Parsing trips ({len(trips_csv.index)} rows)...')

    route_id_by_trip_id = dict(zip(trips_csv.trip_id, trips_csv.route_id))

    del trips_csv

    print('Reading stop_times.csv...')
    stop_times_csv = pd.read_csv(
        mkpath(raw_data_path, 'stop_times.csv'),
        usecols=tuple(STOP_TIMES_COLUMNS.keys()),
        dtype=STOP_TIMES_COLUMNS,
        na_filter=False
    )

    print(f'Parsing stop times ({len(stop_times_csv.index)} rows)...')

    routes: dict[str, dict[str, list[Stop]]] = defaultdict(lambda: defaultdict(list))  # {route_id: {trip_id: [stops]}}

    stop_times_csv.sort_values('stop_sequence', inplace=True)
    stop_times_csv.drop(columns='stop_sequence', inplace=True)

    for row in stop_times_csv.itertuples(index=False):
        if row.stop_id not in stations_by_id:
            print(f'Stop {row.stop_id} not found')
            continue  # TODO

        arrival_hours, arrival_minutes, arrival_seconds = map(int, row.arrival_time.split(':'))
        departure_hours, departure_minutes, departure_seconds = map(int, row.departure_time.split(':'))

        arrival_days, arrival_hours = divmod(arrival_hours, 24)
        departure_days, departure_hours = divmod(departure_hours, 24)

        arrival = arrival_days * 86400 + arrival_hours * 3600 + arrival_minutes * 60 + arrival_seconds
        departure = departure_days * 86400 + departure_hours * 3600 + departure_minutes * 60 + departure_seconds

        # stops_for_trip[row.trip_id].append(
        routes[route_id_by_trip_id[row.trip_id]][row.trip_id].append(
            Stop(
                station=stations_by_id[row.stop_id],
                time=StopTime(data_timestamp + arrival, data_timestamp + departure)
            )
        )

    del stations_by_id
    del stop_times_csv

    if DEBUG:
        print('Checking stop times...')  # Checking departure and arrival time consistency
        for trips in routes.values():
            for stops in trips.values():
                for i in range(1, len(stops)):
                    assert stops[i - 1].time.departure_time <= stops[i].time.arrival_time, (
                        f'Stop {stops[i - 1].station.id}/{stops[i - 1].station.name} ({stops[i - 1].time.departure_time}) '
                        f'and stop {stops[i].station.id}/{stops[i].station.name} ({stops[i].time.arrival_time}) '
                        f'are not in the correct order in trip {row.trip_id}'
                    )

    print('Reading routes.csv...')
    routes_csv = pd.read_csv(
        mkpath(raw_data_path, 'routes.csv'),
        usecols=tuple(ROUTES_COLUMNS.keys()),
        dtype=ROUTES_COLUMNS,
        na_filter=False
    )

    route_data_by_id: dict[str, tuple[str, int]] = {}

    for row in routes_csv.itertuples(index=False):
        short_name = prettify_names(row.route_short_name)
        long_name = prettify_names(row.route_long_name)
        if short_name:
            route_name = short_name
        elif long_name:
            route_name = long_name
        else:
            route_name = ''
            print(f'Route {row.route_id} has no name')

        route_type = row.route_type
        if route_type == 109:
            route_type = 2

        assert route_type in (0, 1, 2, 3, 4, 5, 6, 7, 11, 12), f'Route {row.route_id} has unknown type {route_type}'

        route_data_by_id[row.route_id] = (route_name, route_type)

    print(f'Splitting similar trips in each route...')

    route_similar_trips_dict: dict[str, dict[tuple[Station], list[str]]] = defaultdict(lambda: defaultdict(list))
    #                                                                       {route_id: {stations: [trip_ids]}}

    for route_id, trips in routes.items():
        for trip_id, stops in trips.items():
            route_similar_trips_dict[route_id][tuple(stop.station for stop in stops)].append(trip_id)

    print(f'Flattening similar trips...')

    similar_trips: list[Route] = []

    for route_id, route_similar_trips in route_similar_trips_dict.items():
        for stations, trip_ids in route_similar_trips.items():
            similar_trips.append(
                Route(
                    route_name=route_data_by_id[route_id][0],
                    route_type=route_data_by_id[route_id][1],
                    station_ids=[station.id for station in stations],
                    trips=[
                        Trip([
                            stop.time
                            for stop in routes[route_id][trip_id]
                        ]) for trip_id in trip_ids
                    ]
                )
            )

    del route_data_by_id
    del route_similar_trips_dict

    print('Total amount of routes:', len(similar_trips))
    print('Average amount of trips per route:', round(
        sum(len(route.trips) for route in similar_trips) / len(similar_trips)
    ))

    print('Writing result...')
    dump_json(similar_trips, mkpath(data_path, 'trips.json'), indent_depth=4)


if __name__ == '__main__':
    convert_trips()
