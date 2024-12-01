from collections import defaultdict

import json
import os

import pandas as pd

from src.types import Route, Stop, StopTime
from src.utils import mkpath, dump_json


RAW_DATA_PATH = mkpath('../data/raw/gtfs_hamburg')
DATA_PATH = mkpath('../data/gtfs_hamburg')

TRIPS_COLUMNS = {
    'route_id': str,
    'trip_id': int
}
STOP_TIMES_COLUMNS = {
    'trip_id': int,
    'stop_id': str,
    'arrival_time': str,
    'departure_time': str,
    'stop_sequence': int
}


def convert_trips():
    print('Converting trips...')

    assert os.path.isfile(mkpath(DATA_PATH, 'stations_map.json')), 'Run convert_stations.py first'
    with open(mkpath(DATA_PATH, 'stations_map.json'), 'r') as file:
        stations_map = json.load(file)

    # print('Reading trips.csv...')
    # trips_csv = pd.read_csv(
    #     mkpath(RAW_DATA_PATH, 'trips.csv'),
    #     keep_default_na=False,
    #     usecols=tuple(TRIPS_COLUMNS.keys()),
    #     dtype=TRIPS_COLUMNS
    # )
    print('Reading stop_times.csv...')
    stop_times_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'stop_times.csv'),
        keep_default_na=False,
        usecols=tuple(STOP_TIMES_COLUMNS.keys()),
        dtype=STOP_TIMES_COLUMNS
    )

    print(f'Processing stop times ({len(stop_times_csv.index)} rows)...')

    stops_for_trip = defaultdict(list)

    print('Parsing stop times...')
    for row in stop_times_csv.itertuples(index=False):
        stops_for_trip[row.trip_id].append([
            tuple(map(int, row.arrival_time.split(':'))),
            tuple(map(int, row.departure_time.split(':'))),
            row.stop_id,
            row.stop_sequence
        ])

    print('Sorting and converting stop times...')
    for stops in stops_for_trip.values():
        stops.sort(key=lambda stop_data: stop_data[-1])  # Sorting by stop_sequence

        for i, stop in enumerate(stops):
            stops[i] = Stop(
                time=StopTime(
                    stop[0],
                    stop[1]
                ),
                station=stations_map.get(stop[2], stop[2])
            )

        # Checking departure and arrival time consistency
        for i in range(1, len(stops)):
            assert stops[i - 1].time.departure_time <= stops[i].time.arrival_time, (
                f'Stop {stops[i - 1].station.id}/{stops[i - 1].station.name} ({stops[i - 1].time.departure_time}) '
                f'and stop {stops[i].station.id}/{stops[i].station.name} ({stops[i].time.arrival_time}) '
                f'are not in the correct order in trip {row.trip_id}'
            )

    print(f'Arranging trips by route...')

    similar_trips_dict = defaultdict(list)

    for trip_id, stops in stops_for_trip.items():
        stop_ids = tuple(stop.id for stop in stops)
        similar_trips_dict[stop_ids].append(trip_id)

    similar_trips = []

    for trip_ids in similar_trips_dict.values():
        similar_trips.append([stops_for_trip[trip_id] for trip_id in trip_ids])

    print('Total amount of routes:', len(similar_trips))
    print('Average amount of trips per route:', round(
        sum(len(route) for route in similar_trips) / len(similar_trips)
    ))

    # Checking
    for route in similar_trips:
        intial_trip_stop_ids = tuple(stop.id for stop in route[0])
        for trip in route:
            cur_trip_stop_ids = tuple(stop.id for stop in trip)
            assert cur_trip_stop_ids == intial_trip_stop_ids, (
                f'Trip {cur_trip_stop_ids} does not match the initial trip {intial_trip_stop_ids}'
            )

    # print(similar_trips)

    print('Writing result...')
    dump_json(similar_trips, mkpath(DATA_PATH, 'trips.json'), indent_depth=3)


if __name__ == '__main__':
    convert_trips()
