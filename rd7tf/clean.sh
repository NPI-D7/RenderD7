#!/bin/bash

#Clean old build
echo "Cleaning..."
make clean
rm -rf libs/*
cd ..
make clean
echo "Done!"