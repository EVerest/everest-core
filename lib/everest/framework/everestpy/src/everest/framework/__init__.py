__version__ = '0.25.0'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
