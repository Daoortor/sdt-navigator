from src.convert_transfers import convert_transfers
from src.convert_stations import convert_stations
from src.convert_trips import convert_trips


if __name__ == '__main__':
    convert_stations()
    print()
    convert_trips()
    print()
    convert_transfers()
