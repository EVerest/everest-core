#!/bin/sh
nanopb_generator.py -L "#include <everest/3rd_party/nanopb/%s>" -I . -D . umwc.proto
