__version__ = '0.22.0'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
