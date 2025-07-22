__version__ = '0.22.2'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
