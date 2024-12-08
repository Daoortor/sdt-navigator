from typing import NamedTuple, NewType


class Station(NamedTuple):
    name: str
    id: str
    # lat: float
    # lon: float


class StopTime(NamedTuple):
    arrival_time: int  # Timestamp
    departure_time: int  # Timestamp


class Stop(NamedTuple):
    station: Station
    time: StopTime


Trip = NewType('Trip', list[StopTime])


class Route(NamedTuple):
    route_name: str
    route_type: int
    station_ids: list[str]
    trips: list[Trip]


class Transfer(NamedTuple):
    start_id: Station.id
    end_id: Station.id
    time: int
