#!/bin/sh
nanopb_generator.py -L "#include <everest/3rd_party/nanopb/%s>" -I . -D . lo2hi.proto hi2lo.proto
