from _ctypes import PyObj_FromPtr
from itertools import islice
import json
import os
import re


# https://stackoverflow.com/a/13252112/9759280
class NoIndent(object):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return f'NoIndent({repr(self.value)})'


class CustomJsonEncoder(json.JSONEncoder):
    FORMAT_SPEC = '@@{}@@'
    regex = re.compile(FORMAT_SPEC.format(r'(\d+)'))

    def __init__(self, **kwargs):
        # Save copy of any keyword argument values needed for use here.
        self.__sort_keys = kwargs.get('sort_keys', None)
        super(CustomJsonEncoder, self).__init__(**kwargs)

    def default(self, obj):
        return (self.FORMAT_SPEC.format(id(obj)) if isinstance(obj, NoIndent)
                else super(CustomJsonEncoder, self).default(obj))

    def encode(self, obj):
        format_spec = self.FORMAT_SPEC  # Local var to expedite access.
        json_repr = super(CustomJsonEncoder, self).encode(obj)  # Default JSON.

        # Replace any marked-up object ids in the JSON repr with the
        # value returned from the json.dumps() of the corresponding
        # wrapped Python object.
        for match in self.regex.finditer(json_repr):
            # see https://stackoverflow.com/a/15012814/355230
            id = int(match.group(1))
            no_indent = PyObj_FromPtr(id)
            json_obj_repr = json.dumps(no_indent.value, sort_keys=self.__sort_keys)

            # Replace the matched id string with json formatted representation
            # of the corresponding Python object.
            json_repr = json_repr.replace('"{}"'.format(format_spec.format(id)), json_obj_repr)

        return json_repr


def mkpath(*paths: str) -> str:
    return os.path.normpath(os.path.join(*paths))


def load_json(path: str) -> dict | list:
    with open(path, 'r', encoding='utf-8') as file:
        return json.load(file)


def make_noindent(obj: dict | list | tuple, depth: int) -> dict | list | NoIndent:
    if isinstance(obj, dict):
        if depth == 0:
            return NoIndent(obj)
        return {key: make_noindent(value, depth - 1) for key, value in islice(obj.items(), 10)}

    if isinstance(obj, list | tuple):
        if depth == 0:
            return NoIndent(obj)
        return [make_noindent(value, depth - 1) for value in islice(obj, 10)]

    return obj


def dump_json(data, path: str, indent_depth=float('inf')):
    filename = os.path.basename(path)
    filename_pretty = os.path.splitext(filename)[0] + '.pretty.json'

    # with open(path, 'w', encoding='utf-8') as file:
    #     json.dump(data, file, ensure_ascii=False, separators=(',', ':'))

    with open(mkpath(os.path.dirname(path), filename_pretty), 'w', encoding='utf-8') as file:
        # json.dump(data, file, ensure_ascii=False, indent='\t')
        file.write(json.dumps(make_noindent(data, indent_depth), cls=CustomJsonEncoder, indent='\t'))


if __name__ == '__main__':  # Tests
    dump_json({'a': [1, 2], 'b': 3}, '../test.json', indent_depth=1)
    os.remove('../test.json')
    os.remove('../test.pretty.json')
