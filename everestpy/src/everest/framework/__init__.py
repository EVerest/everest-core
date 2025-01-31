__version__ = '0.20.0'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
