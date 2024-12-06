import pandas as pd

from src.ts_types import Station
from src.grammar import prettify_names
from src.utils import mkpath, dump_json


STATIONS_COLUMNS = {
    'stop_id': str,
    'stop_name': str,
    # 'stop_lat': float,
    # 'stop_lon': float,
    'parent_station': str
}


def convert_stations(raw_data_path: str, data_path: str):
    print('Converting stations...')

    print('Reading stops.csv...')
    stops_csv = pd.read_csv(
        mkpath(raw_data_path, 'stops.csv'),
        keep_default_na=False,
        usecols=tuple(STATIONS_COLUMNS.keys()),
        dtype=STATIONS_COLUMNS,
        na_filter=False
    )

    print(f'Processing stops ({len(stops_csv.index)} rows)...')

    stations_by_name: dict[str, Station] = {}
    stations_ids: set[str] = set()  # For checking parent_station field

    stations_id_map: dict[str, str] = {}

    for row in stops_csv.itertuples(index=False):
        if row.parent_station:
            stations_id_map[row.stop_id] = row.parent_station
            continue

        stop_name = prettify_names(row.stop_name)

        if stop_name in stations_by_name:
            stations_id_map[row.stop_id] = stations_by_name[stop_name].id
            continue

        stations_by_name[stop_name] = Station(
            row.stop_name,
            row.stop_id,
            # row.stop_lat,
            # row.stop_lon
        )
        stations_ids.add(row.stop_id)

    # Checking: all parent_station stop ids should be stored
    for row in stops_csv.itertuples(index=False):
        if row.parent_station:
            assert stations_id_map.get(row.parent_station, row.parent_station) in stations_ids, f'Parent station not found: {row.parent_station}'

    stations: list[Station] = sorted(stations_by_name.values())

    print('Total amount of stations:', len(stations_by_name))

    print('Writing result...')
    dump_json(stations, mkpath(data_path, 'stations.json'), indent_depth=1)
    dump_json(stations_id_map, mkpath(data_path, 'stations_map.json'))


if __name__ == '__main__':
    convert_stations()
