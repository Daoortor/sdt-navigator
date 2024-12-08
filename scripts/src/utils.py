from typing import TextIO
import json
import os

import orjson


ROOT_PATH = os.path.dirname(os.path.abspath(__file__)) + '/../../'


def mkpath(*paths: str) -> str:
    return os.path.normpath(os.path.join(*paths))


def mkpath_root(*paths: str) -> str:
    return mkpath(ROOT_PATH, *paths)


# def load_json(path: str) -> dict | list:
#     with open(path, 'r', encoding='utf-8') as file:
#         return json.load(file)


def _orjson_default(obj):
    if isinstance(obj, tuple):
        return list(obj)
    return str(obj)


def cursom_json_decoder(result: TextIO, obj, indent_depth=0, current_indent=0) -> str:
    if indent_depth == 0:
        result.write(json.dumps(obj, ensure_ascii=False))
        return

    current_indent += 1

    if isinstance(obj, dict):
        result.write('{\n')
        for key, value in obj.items():
            result.write('\t' * current_indent)
            result.write(json.dumps(key, ensure_ascii=False))
            result.write(': ')
            cursom_json_decoder(result, value, indent_depth - 1, current_indent)
            result.write(',\n')
        result.write('\t' * (current_indent - 1))
        result.write('}')
        return

    if isinstance(obj, list | tuple):
        result.write('[\n')
        for value in obj:
            result.write('\t' * current_indent)
            cursom_json_decoder(result, value, indent_depth - 1, current_indent)
            result.write(',\n')
        result.write('\t' * (current_indent - 1))
        result.write(']')
        return

    result.write(json.dumps(obj, ensure_ascii=False))


def dump_json(data, path: str, indent_depth=float('inf')):
    DEBUG = (os.environ.get('_CONVERTER_DEBUG') == '1')

    filename = os.path.basename(path)
    filename_pretty = os.path.splitext(filename)[0] + '.pretty.json'

    if not os.path.isdir(os.path.dirname(path)):
        os.makedirs(os.path.dirname(path))

    # print('Writing compressed file...')
    with open(path, 'wb') as file:
        file.write(orjson.dumps(data, default=_orjson_default))
        # file.write(cursom_json_decoder(data, indent_depth))

    if DEBUG:
        # print('Writing pretty file...')
        with open(mkpath(os.path.dirname(path), filename_pretty), 'w', encoding='utf-8') as file:
            # json.dump(data, file, ensure_ascii=False, indent='\t')
            cursom_json_decoder(file, data, indent_depth)


if __name__ == '__main__':  # Tests
    dump_json({'a': [1, 2], 'b': 3}, '../test.json', indent_depth=1)
    os.remove('../test.json')
    os.remove('../test.pretty.json')
