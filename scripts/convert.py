from convert_transfers import convert_transfers
from convert_stations import convert_stations
from convert_trips import convert_trips


if __name__ == '__main__':
    convert_stations()
    print()
    convert_trips()
    print()
    convert_transfers()
