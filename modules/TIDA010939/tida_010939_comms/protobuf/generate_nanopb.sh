#!/bin/sh
nanopb_generator.py -L "#include <everest/3rd_party/nanopb/%s>" -I . -D . tida010939.proto
