from datetime import date

from src.utils import mkpath
from src.convert_transfers import convert_transfers
from src.convert_stations import convert_stations
from src.convert_trips import convert_trips


date = date(2020, 3, 20)  # GTFS data date: 20 March 2020

raw_data_path = mkpath('../data/raw/gtfs_hamburg')
data_path = mkpath('../data/gtfs_hamburg')


if __name__ == '__main__':
    convert_stations(raw_data_path, data_path)
    print()
    convert_trips(raw_data_path, data_path, date)
    print()
    convert_transfers(raw_data_path, data_path)
