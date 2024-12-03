from collections import defaultdict
import os

import simdjson as json
import pandas as pd

from src.converter_types import Station, StopTime, Stop, Trip, Route
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

    assert os.path.isfile(mkpath(DATA_PATH, 'stations.json')), 'Run convert_stations.py first'
    assert os.path.isfile(mkpath(DATA_PATH, 'stations_map.json')), 'Run convert_stations.py first'

    with open(mkpath(DATA_PATH, 'stations.json'), 'r', encoding='utf-8') as file:
        stations_by_id = {station.id: station for station in map(Station._make, json.load(file))}

    with open(mkpath(DATA_PATH, 'stations_map.json'), 'r', encoding='utf-8') as file:
        stations_by_id.update(
            (map_from, stations_by_id[map_to])
            for map_from, map_to in json.load(file).items()
        )

    print('Reading stop_times.csv...')
    stop_times_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'stop_times.csv'),
        keep_default_na=False,
        usecols=tuple(STOP_TIMES_COLUMNS.keys()),
        dtype=STOP_TIMES_COLUMNS,
        na_filter=False
    )

    stops_for_trip: dict[str, list[tuple[int, Stop]]] = defaultdict(list)

    print(f'Parsing stop times ({len(stop_times_csv.index)} rows)...')
    for row in stop_times_csv.itertuples(index=False):
        assert row.stop_id in stations_by_id, f'Stop {row.stop_id} not found'
        stops_for_trip[row.trip_id].append((
            row.stop_sequence,
            Stop(
                station=stations_by_id[row.stop_id],
                time=StopTime(
                    tuple(map(int, row.arrival_time.split(':'))),
                    tuple(map(int, row.departure_time.split(':'))),
                )
            )
        ))

    print('Sorting and converting stop times...')
    for stops in stops_for_trip.values():
        stops.sort(key=lambda stop_data: stop_data[0])  # Sorting by stop_sequence

        for i, stop in enumerate(stops):  # Removing stop_sequence
            stops[i] = stop[1]

        # Checking departure and arrival time consistency
        for i in range(1, len(stops)):
            assert stops[i - 1].time.departure_time <= stops[i].time.arrival_time, (
                f'Stop {stops[i - 1].station.id}/{stops[i - 1].station.name} ({stops[i - 1].time.departure_time}) '
                f'and stop {stops[i].station.id}/{stops[i].station.name} ({stops[i].time.arrival_time}) '
                f'are not in the correct order in trip {row.trip_id}'
            )

    # stops_for_trip: dict[str, list[Stop]]

    print(f'Arranging trips by route...')

    similar_trips_dict: dict[tuple[Station], list[str]] = defaultdict(list)  # {stations: [trip_ids]}

    for trip_id, stops in stops_for_trip.items():
        similar_trips_dict[tuple(stop.station for stop in stops)].append(trip_id)

    similar_trips: Route = []

    for stations, trip_ids in similar_trips_dict.items():
        similar_trips.append(
            Route(
                stations=stations,
                trips=[
                    Trip([
                        stop.time
                        for stop in stops_for_trip[trip_id]
                    ]) for trip_id in trip_ids
                ]
            )
        )

    print('Total amount of routes:', len(similar_trips))
    print('Average amount of trips per route:', round(
        sum(len(route.trips) for route in similar_trips) / len(similar_trips)
    ))

    print('Writing result...')
    dump_json(similar_trips, mkpath(DATA_PATH, 'trips.json'), indent_depth=4)


if __name__ == '__main__':
    convert_trips()
