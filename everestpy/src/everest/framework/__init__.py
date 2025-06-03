__version__ = '0.21.1'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
