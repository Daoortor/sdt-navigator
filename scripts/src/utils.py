from io import StringIO
import json
import os


def mkpath(*paths: str) -> str:
    return os.path.normpath(os.path.join(*paths))


def load_json(path: str) -> dict | list:
    with open(path, 'r', encoding='utf-8') as file:
        return json.load(file)


def cursom_json_decoder(obj, indent_depth=0, current_indent=0):
    if indent_depth == 0:
        return json.dumps(obj, ensure_ascii=False)

    current_indent += 1

    if isinstance(obj, dict):
        result = StringIO()
        result.write('{\n')
        for key, value in obj.items():
            result.write(f'{"\t" * current_indent}{json.dumps(key, ensure_ascii=False)}: ')
            result.write(cursom_json_decoder(value, indent_depth - 1, current_indent))
            result.write(',\n')
        result.write(f'{"\t" * (current_indent - 1)}}}')
        return result.getvalue()

    if isinstance(obj, list | tuple):
        result = StringIO()
        result.write('[\n')
        for value in obj:
            result.write(f'{"\t" * current_indent}{cursom_json_decoder(value, indent_depth - 1, current_indent)}')
            result.write(',\n')
        result.write(f'{"\t" * (current_indent - 1)}]')
        return result.getvalue()

    return json.dumps(obj, ensure_ascii=False)


def dump_json(data, path: str, indent_depth=float('inf')):
    filename = os.path.basename(path)
    filename_pretty = os.path.splitext(filename)[0] + '.pretty.json'

    # print('Writing compressed file...')
    with open(path, 'w', encoding='utf-8') as file:
        json.dump(data, file, ensure_ascii=False, separators=(',', ':'))
        # file.write(cursom_json_decoder(data, indent_depth))

    # print('Writing pretty file...')
    with open(mkpath(os.path.dirname(path), filename_pretty), 'w', encoding='utf-8') as file:
        # json.dump(data, file, ensure_ascii=False, indent='\t')
        file.write(cursom_json_decoder(data, indent_depth))


if __name__ == '__main__':  # Tests
    dump_json({'a': [1, 2], 'b': 3}, '../test.json', indent_depth=1)
    os.remove('../test.json')
    os.remove('../test.pretty.json')
