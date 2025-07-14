__version__ = '0.22.1'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
