#!/bin/sh
nanopb_generator.py -L "#include <nanopb/%s>" -I . -D . phyverso.proto
