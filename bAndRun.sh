#!/bin/bash

cd build
cmake ..
make -j4
./robot
cd ..