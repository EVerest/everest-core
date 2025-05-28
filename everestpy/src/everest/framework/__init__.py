__version__ = '0.21.0'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
