#!/bin/bash
#Build Tools
echo "Generate Directorys"
mkdir -p bin
echo "Building file2array"
g++ -static -o bin/file2array file2array/file2array.cpp
echo "Building rd7f2afix"
g++ -static -o bin/rd7f2afix rd7f2afix/rd7f2afix.cpp

echo "Finished!" 
