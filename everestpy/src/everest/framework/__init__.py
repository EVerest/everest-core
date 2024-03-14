__version__ = '0.0.4'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
