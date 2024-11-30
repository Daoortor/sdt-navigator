from collections import defaultdict
from typing import NamedTuple

import pandas as pd

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


class Stop(NamedTuple):
    id: str
    arrival_time: tuple[int, int, int]
    departure_time: tuple[int, int, int]


def convert_trips():
    print('Converting trips...')

    print('Reading trips.csv...')
    trips_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'trips.csv'),
        keep_default_na=False,
        usecols=tuple(TRIPS_COLUMNS.keys()),
        dtype=TRIPS_COLUMNS
    )
    print('Reading stop_times.csv...')
    stop_times_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'stop_times.csv'),
        keep_default_na=False,
        usecols=tuple(STOP_TIMES_COLUMNS.keys()),
        dtype=STOP_TIMES_COLUMNS
    )

    print(f'Processing stop times ({len(stop_times_csv.index)} rows)...')

    stop_times_by_trip = defaultdict(list)

    print('Parsing stop times...')
    for row in stop_times_csv.itertuples(index=False):
        stop_times_by_trip[row.trip_id].append([
            row.stop_id,
            tuple(map(int, row.arrival_time.split(':'))),
            tuple(map(int, row.departure_time.split(':'))),
            row.stop_sequence
        ])

    print('Sorting and converting stop times...')
    for stop_times in stop_times_by_trip.values():
        stop_times.sort(key=lambda stop_data: stop_data[-1])
        for cur_stop in stop_times:
            cur_stop.pop()

        for i, stop in enumerate(stop_times):
            stop_times[i] = Stop(*stop)

        for i in range(1, len(stop_times)):
            assert stop_times[i - 1].departure_time <= stop_times[i].arrival_time, (
                f'Stop {stop_times[i - 1].id} ({stop_times[i - 1].departure_time}) '
                f'and stop {stop_times[i].id} ({stop_times[i].arrival_time}) '
                f'are not in the correct order in trip {row.trip_id}'
            )

    # print(stop_times_by_trip)

    print(f'Processing trips ({len(trips_csv.index)} rows)...')

    trips_by_route = defaultdict(list)

    for row in trips_csv.itertuples(index=False):
        assert row.trip_id in stop_times_by_trip, f"Trip {row.trip_id} has no stop times"
        trips_by_route[row.route_id].append(stop_times_by_trip[row.trip_id])

    print('Writing result...')

    dump_json(trips_by_route, mkpath(DATA_PATH, 'trips.json'), indent_depth=3)


if __name__ == '__main__':
    convert_trips()
