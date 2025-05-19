__version__ = '0.20.4'

try:
    from .everestpy import *
except ImportError:
    from everestpy import *
