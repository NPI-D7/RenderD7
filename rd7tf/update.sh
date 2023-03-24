#!/bin/bash

#Clean old build
echo "Cleaning..."
rm -rf libs/*
#Format to LLVM
echo "Format to LLVM"
cd ..
./cformat.sh
#Compile RenderD7
echo "Compiling RenderD7"
make
#Copy new build
echo "Copy New Build"
cp -r include rd7tf/libs/
cp -r lib rd7tf/libs/
#Build Test Framework
echo "Building Test Framework"
cd rd7tf
make

echo "Done!"