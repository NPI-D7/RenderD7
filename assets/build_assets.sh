#!/bin/bash

echo "Create Directorys"
mkdir -p build
mkdir -p code

echo "Generating Spritesheets"
cd resources

for file in *.t3s
do
    filename=$(basename "$file" .t3s).t3x
    tex3ds -o "../build/$filename" -i "${file}"
done

echo "Converting Spritesheets into C++ Code"
cd ../code

for file in ../build/*.t3x
do
    name="${file##*/}"
    name="${name%.*}"
    ../../tools/bin/file2array "${file}" "${name}"
done

echo "Fix The Output of file2array"
for file in ./*.cpp
do
    ../../tools/bin/rd7f2afix "${file}" "renderd7"
done
echo "Finished!"