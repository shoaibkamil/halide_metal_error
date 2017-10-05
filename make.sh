#!/bin/bash

HALIDE_DIR=/Users/kamil/Documents/work/adobe/Halide

rm select.o select.h select_gen main
c++ -fno-rtti select.cpp $HALIDE_DIR/tools/GenGen.cpp -g -std=c++11 -I $HALIDE_DIR/include -L $HALIDE_DIR/bin -lHalide -lpthread -ldl -o select_gen
./select_gen  -f select -g select_err -e o,h -o . target=x86-64-osx-metal-debug
./select_gen  -f select_cpu -g select_err -e o,h -o . target=x86-64-osx-debug
c++ -fno-rtti main.cpp select.o select_cpu.o -g -std=c++11 -I $HALIDE_DIR/include -L $HALIDE_DIR/bin -lHalide -lpthread -ldl -framework Metal -framework Foundation -o main
