import os

from src.utils import mkpath


RAW_DATA_PATH = mkpath('../data/raw/gtfs_hamburg')


for filename in os.listdir(RAW_DATA_PATH):
    if filename.endswith('.txt'):
        csv_filename = filename[:-4] + '.csv'
        csv_filepath = mkpath(RAW_DATA_PATH, csv_filename)
        if os.path.exists(csv_filepath):
            os.remove(csv_filepath)
        os.rename(mkpath(RAW_DATA_PATH, filename), csv_filepath)
