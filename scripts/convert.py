from datetime import date

from src.utils import mkpath
from src.convert_transfers import convert_transfers
from src.convert_stations import convert_stations
from src.convert_trips import convert_trips
from txt_to_csv import txt_to_csv


cities = [
    # ('rome', date(2020, 3, 24)),  # GTFS data date: 24 March 2020
    ('hamburg', date(2020, 3, 20))  # GTFS data date: 20 March 2020
]

if __name__ == '__main__':
    for city, data_date in cities:
        print(f'>>> Converting {city} GTFS data...')
        raw_data_path = mkpath(f'../data/raw/gtfs_{city}')
        data_path = mkpath(f'../data/gtfs_{city}')

        print()

        txt_to_csv(raw_data_path)
        print()

        convert_stations(raw_data_path, data_path)
        print()
        convert_trips(raw_data_path, data_path, data_date)
        print()
        convert_transfers(raw_data_path, data_path)
