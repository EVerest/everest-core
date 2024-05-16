__version__ = '0.0.5'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
