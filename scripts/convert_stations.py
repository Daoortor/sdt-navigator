import pandas as pd

from src.utils import mkpath, dump_json


RAW_DATA_PATH = mkpath('../data/raw/gtfs_hamburg')
DATA_PATH = mkpath('../data/gtfs_hamburg')

STATIONS_COLUMNS = {
    'stop_id': str,
    'stop_name': str,
    'stop_lat': float,
    'stop_lon': float,
    'parent_station': str
}


def convert_stations():
    print('Converting stations...')

    print('Reading stops.csv...')
    stops_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'stops.csv'),
        keep_default_na=False,
        usecols=tuple(STATIONS_COLUMNS.keys()),
        dtype=STATIONS_COLUMNS
    )

    print(f'Processing stops ({len(stops_csv.index)} rows)...')

    stations_by_name = {}
    stations_ids = set()  # For checking parent_station field

    stations_id_map = {}

    for row in stops_csv.itertuples(index=False):
        if row.parent_station:
            stations_id_map[row.stop_id] = row.parent_station
            continue

        assert row.stop_name not in stations_by_name, f"Stop name {row.stop_name} already exists"

        stations_by_name[row.stop_name] = (
            row.stop_id,
            row.stop_lat,
            row.stop_lon
        )
        stations_ids.add(row.stop_id)

    # All parent_station stop ids should be stored
    for row in stops_csv.itertuples(index=False):
        if row.parent_station:
            assert row.parent_station in stations_ids, row.parent_station

    stations = []
    for stations_name, stations_data in stations_by_name.items():
        stations.append((stations_name, *stations_data))

    stations.sort()

    print('Total amount of stations:', len(stations_by_name))

    print('Writing result...')
    dump_json(stations, mkpath(DATA_PATH, 'stations.json'), indent_depth=1)
    dump_json(stations_id_map, mkpath(DATA_PATH, 'stations_map.json'))


if __name__ == '__main__':
    convert_stations()
