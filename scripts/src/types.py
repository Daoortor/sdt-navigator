from typing import NamedTuple


class Station(NamedTuple):
    id: str
    name: str
    lat: float
    lon: float


class StopTime(NamedTuple):
    arrival_time: tuple[int, int, int]
    departure_time: tuple[int, int, int]


class Stop(NamedTuple):
    station: Station
    time: StopTime


type Trip = list[StopTime]


class Route(NamedTuple):
    stations: list[Station]
    trips: list[Trip]
