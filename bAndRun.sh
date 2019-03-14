#!/bin/bash

# currentPath=`pwd`

if [ ! -d "./build" ]; then
	mkdir build
fi

cd build
	cmake -DCMAKE_BUILD_TYPE=Debug ..
	make -j4
	printf "\n Done Building \n \n"
	./execute
# ./robot_train clear_cache
# ./robot_train import /home/rubend/Documents/RobotTrainData
# ./robot_train create_data box.data
cd ..
