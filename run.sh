#!/bin/bash
set -e
cmake -S . -B build
cmake --build build --target clean_avionics
cmake --build build --target avionics
./avionics
