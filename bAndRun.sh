#!/bin/bash

cd build
cmake ..
make -j4
printf "\n Done Building \n \n"
./robot
cd ..
