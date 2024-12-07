<h1 align="center">Multimodal Navigation System</h1>
<h6 align="center">Final project for the Advanced Algorithms and Data Structures course at Constructor University</h6>

## Setup

This project is developed in C++ and Python and has been primarily tested on Linux. For optimal compatibility and performance, we recommend using a Linux environment. The following instructions are specifically tailored for Linux systems:

1. Install [Python 3](https://www.python.org) (tested on `Python 3.12`, newer versions are preferable)
2. Install [CMake](https://cmake.org) (e.g. `sudo apt install cmake` for Ubuntu)
3. Install Qt 6 (e.g. `sudo apt install qt6-base-dev` for Ubuntu)
4. Install Python packages by running `pip install -r scripts/requirements.txt` in the project's root directory

<details>
<summary>Ready-to-use commands for Ubuntu</summary>

```bash
sudo apt update
sudo apt install python3 python3-pip cmake qt6-base-dev
pip install -r scripts/requirements.txt
```

</details>

## Preparation

Our project relies on the public transit data in the [GTFS](https://en.wikipedia.org/wiki/GTFS) format. For development purposes, some historical GTFS data can be obtained from [transitfeeds.com](https://transitfeeds.com) or [mobilitydatabase.org](https://mobilitydatabase.org).

GTFS in `zip`-archived format should be placed in the [`data/raw`](data/raw) directory.

We can also precompile our main program:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build --parallel
```

## Running

Running the project consists of two steps:

1. First we need to prepare the data:
	```bash
	python scripts/convert.py
	```

2. TODO: ещё не понятно, что запускать
<!-- 2. Now we run the main program:
	```bash
	./build/sdtmaps
	``` -->

## Testing

Tests are [automatically run by GitHub Actions](../../actions). To run them locally:

1. Prepare the data as shown above:
	```bash
	python scripts/convert.py
	```

2. Run the tests:
	```bash
	ctest -V --output-on-failure
	```

## License

This project is distributed under the terms of the [GPL-3.0 License](LICENSE)

Please note that this is a research-focused project and is not yet intended for production use