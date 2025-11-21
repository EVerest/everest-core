__version__ = '0.24.0'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
