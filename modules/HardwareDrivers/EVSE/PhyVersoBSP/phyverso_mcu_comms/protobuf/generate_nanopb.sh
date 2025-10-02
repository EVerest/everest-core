#!/bin/sh
nanopb_generator -L "#include <everest/3rd_party/nanopb/%s>" -I . -D . phyverso.proto
