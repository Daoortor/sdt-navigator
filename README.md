<h1 align="center">SDT Navigator</h1>
<h6 align="center">
	<a href="https://docs.google.com/presentation/d/e/2PACX-1vQNlbyWFmbQOSK8aOKznbCmE7JXFppdAYH7fN8-1sdmeVfJIxTH9R3RguKPvG3DqfPTjmCuY1mOAvj4/pub">Final project for the Advanced Algorithms and Data Structures course at Constructor University</a>
</h6>


#### Demo: [@SDT_Navigator_bot](https://t.me/SDT_Navigator_bot)


## Setup

This project is developed in C++ and Python and has been primarily tested on Linux. For optimal compatibility and performance, we recommend using a Linux environment. The following instructions are specifically tailored for Linux systems:

1. Install [Python 3.11](https://www.python.org) or newer version
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

## Running (CLI)

Running the project consists of three steps:

1. First we need to prepare the data:
	```bash
	python3 scripts/convert.py
	```

2. We can now choose a city from the `data` folder and run the program. For example, let's choose Hamburg (GTFS folder: `data/gtfs_hamburg`):
	```bash
	./build/sdt-navigator gtfs_hamburg
	```

3. When the application is loaded, we can search for routes between two stops. For example, let's find a route between the "Bremen Hbf" and "Hamburg Airport (Flughafen)":
	```bash
	$ ./build/sdt-navigator gtfs_hamburg
	Loading transport system from directory "gtfs_hamburg"...
	Transport system loaded

	[SDT Navigator]$ route --start "Bremen Hbf" --end "Hamburg Airport (Flughafen)" --date 2020-03-20 --time 06:00
	Journey from Bremen Hbf to Hamburg Airport (Flughafen)
	Fri Mar 20 06:26:00 2020 - Fri Mar 20 08:09:00 2020

	RAIL RE4
	Bremen Hbf -> Hamburg-Harburg
	Fri Mar 20 06:26:00 2020 — Fri Mar 20 07:29:00 2020

	RAIL RE3
	Hamburg-Harburg -> Hamburg Hbf
	Fri Mar 20 07:31:00 2020 — Fri Mar 20 07:41:00 2020

	SUBURBANRAILWAY S1
	Hamburg Hbf -> Hamburg Airport (Flughafen)
	Fri Mar 20 07:44:00 2020 — Fri Mar 20 08:09:00 2020
	```

## Telegram bot

To run the Telegram bot, you need to create a `.token` file in the [`scripts`](scripts) folder and put your Telegram bot token there.

After that, you can run the bot:
```bash
python3 scripts/bot.py
```


## Testing

Tests are [automatically run by GitHub Actions](../../actions). To run them locally:

1. Prepare the data as shown above:
	```bash
	python3 scripts/convert.py
	```

2. Run the tests:
	```bash
	ctest --test-dir build -V --output-on-failure
	```

## License

This project is distributed under the terms of the [GPL-3.0 License](LICENSE)

Please note that this is a research-focused project and is not yet intended for production use