import os

from src.utils import mkpath, mkpath_root


def txt_to_csv(folder_path: str):
    for filename in os.listdir(folder_path):
        if filename.endswith('.txt'):
            csv_filepath = mkpath(folder_path, filename[:-4] + '.csv')
            if os.path.exists(csv_filepath):
                os.remove(csv_filepath)
            os.rename(mkpath(folder_path, filename), csv_filepath)


if __name__ == '__main__':
    txt_to_csv(mkpath_root('data/raw/gtfs_hamburg'))
