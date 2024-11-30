from typing import NamedTuple
import pandas as pd

from src.utils import mkpath


RAW_DATA_PATH = mkpath('../data/raw/gtfs_hamburg')
DATA_PATH = mkpath('../data/gtfs_hamburg')

ROUTE = {
	# TODO
}


def convert_routes():
    print('Converting routes...')

	print('Reading routes.csv...')
    routes_csv = pd.read_csv(
        mkpath(RAW_DATA_PATH, 'routes.csv'),
        keep_default_na=False,
        usecols=tuple(ROUTE.keys()),
        dtype=ROUTE
    )

    # TODO

    dump_json(routes, mkpath(DATA_PATH, 'routes.json'))


if __name__ == '__main__':
    convert_routes()
