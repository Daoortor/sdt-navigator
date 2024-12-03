from collections import defaultdict
import os

import simdjson as json
import pandas as pd

from src.converter_types import Station, Transfer
from src.utils import mkpath, dump_json


RAW_DATA_PATH = mkpath('../data/raw/gtfs_hamburg')
DATA_PATH = mkpath('../data/gtfs_hamburg')

TRANSFERS_COLUMNS = {
    'from_stop_id': str,
    'to_stop_id': str,
    'transfer_type': int,
    'min_transfer_time': 'Int64'
}


def convert_transfers():
    print('Converting transfers...')

    assert os.path.isfile(mkpath(DATA_PATH, 'stations.json')), 'Run convert_stations.py first'
    assert os.path.isfile(mkpath(DATA_PATH, 'stations_map.json')), 'Run convert_stations.py first'

    with open(mkpath(DATA_PATH, 'stations.json'), 'r', encoding='utf-8') as file:
        stations_by_id = {station.id: station for station in map(Station._make, json.load(file))}

    with open(mkpath(DATA_PATH, 'stations_map.json'), 'r', encoding='utf-8') as file:
        stations_by_id.update(
            (map_from, stations_by_id[map_to])
            for map_from, map_to in json.load(file).items()
        )

    print('Reading transfers.csv...')
    transfers_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'transfers.csv'),
        keep_default_na=False,
        usecols=tuple(TRANSFERS_COLUMNS.keys()),
        dtype=TRANSFERS_COLUMNS,
        na_filter=False
    )

    transfers_by_station: dict[Station, dict[Station, list[int]]] = defaultdict(lambda: defaultdict(list))

    for row in transfers_csv.itertuples(index=False):
        if row.transfer_type != 2:
            continue

        assert row.min_transfer_time is not None, f'Missing min_transfer_time for transfer {row}'

        from_station = min(
            stations_by_id[row.from_stop_id],
            stations_by_id[row.to_stop_id],
            key=lambda station: station.id
        )
        to_station = max(
            stations_by_id[row.from_stop_id],
            stations_by_id[row.to_stop_id],
            key=lambda station: station.id
        )

        if from_station == to_station:
            continue

        time = int(row.min_transfer_time)

        # TODO: First take transfer between main station if it exists,
        # otherwise average of transfers between platforms

        # assert transfers_by_station[from_station].get(to_station, time) == time, \
        #     f'Time conflict for transfer from {from_station} to {to_station}: {transfers_by_station[from_station].get(to_station, time)} != {time}'

        transfers_by_station[from_station][to_station].append(time)

    transfers = []
    for from_station, to_stations in transfers_by_station.items():
        for to_station, times in to_stations.items():
            transfers.append(Transfer(from_station.id, to_station.id, round(sum(times) / len(times))))

    print('Total amount of transfers:', len(transfers))

    print('Writing result...')
    dump_json(transfers, mkpath(DATA_PATH, 'transfers.json'), indent_depth=1)


if __name__ == '__main__':
    convert_transfers()
