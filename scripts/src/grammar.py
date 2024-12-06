import os

from src.utils import mkpath

with open(mkpath(os.path.dirname(__file__), 'grammar_map.txt'), 'r', encoding='utf-8') as file:
    GRAMMAR_MAP = dict(map(lambda line: map(str.strip, line.split(':')), file))


def prettify_names(name: str) -> str:
    result = ' '.join(name.split())

    result = GRAMMAR_MAP.get(result, result)

    result = result.rstrip('.')

    return result
