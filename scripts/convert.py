from shutil import unpack_archive
from datetime import date
import sys
import os

assert sys.version_info >= (3, 11), f'Python 3.11 or later required, got {".".join(map(str, sys.version_info))}'

from src.convert_transfers import convert_transfers
from src.convert_stations import convert_stations
from src.convert_trips import convert_trips
from txt_to_csv import txt_to_csv
from src.utils import mkpath_root


# ====================================================== Settings ======================================================

DEBUG = False

cities = [
    ('hamburg', date(2020, 3, 20)),  # GTFS data date: 20 March 2020
    ('milan', date(2021, 9, 8)),  # GTFS data date: 8 September 2021
    ('paris', date(2021, 3, 25)),  # GTFS data date: 25 March 2021
    ('stockholm', date(2020, 2, 24)),  # GTFS data date: 24 February 2020
]

# ====================================================== Settings ======================================================

if __name__ == '__main__':

    if DEBUG:
        os.environ['_CONVERTER_DEBUG'] = '1'

    archives = []
    for filename in os.listdir(mkpath_root('data/raw')):
        file_path = mkpath_root('data/raw', filename)
        directory_path = mkpath_root('data/raw', filename[:-4])
        if filename.endswith('.zip') and not os.path.isdir(directory_path):
            archives.append((file_path, directory_path))

    if archives:
        print(f'>>> Unpacking ZIP archives...')
        for archive, directory in archives:
            print(f'Unpacking {archive}...')
            unpack_archive(archive, directory)
        print()

    print('>>> Convering TXT file to CSV...')
    for city, data_date in cities:
        raw_data_path = mkpath_root(f'data/raw/gtfs_{city}')
        txt_to_csv(raw_data_path)
    print()

    for city, data_date in cities:
        print(f'>>> Converting {city} GTFS data...')
        raw_data_path = mkpath_root(f'data/raw/gtfs_{city}')
        data_path = mkpath_root(f'data/gtfs_{city}')

        print()
        convert_stations(raw_data_path, data_path)
        print()
        convert_trips(raw_data_path, data_path, data_date)
        print()
        convert_transfers(raw_data_path, data_path)
        print()
