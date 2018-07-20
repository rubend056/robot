#!/bin/bash

cd build

#Intended as templates, you'll need to have imported some images into the cache
nnName="cube_layer"
dataName="all"
./main_train create_data ${dataName}.data
./main_train create_network ${nnName}.net -hidden 2 70 50 -lr 0.7 -lm 0.0001 -bfl 0.05 -sf mse -init all.data
./main_train train ${nnName}.net ${dataName}.data ${nnName}_trained.net -ebr 10

cd ..
