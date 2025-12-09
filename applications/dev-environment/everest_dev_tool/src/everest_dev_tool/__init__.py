__version__ = "0.1.0"

from . import parser

def get_parser():
    return parser.get_parser(__version__)

def main():
    parser.main(get_parser())
