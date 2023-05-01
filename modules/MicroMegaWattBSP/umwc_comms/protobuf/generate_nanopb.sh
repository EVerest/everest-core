#!/bin/sh
nanopb_generator.py -L "#include <nanopb/%s>" -I . -D . lo2hi.proto hi2lo.proto
