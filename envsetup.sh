#!/bin/bash
#Build Tools
echo "Build Tools..."
cd tools
./build_tools.sh
cd ..
#Setup asset-arrays
echo "Generate Asset-Arrays..."
cd assets
./build_assets.sh
echo "Update Files in RenderD7..."
cp -rf code/*.cpp ../source/
cp -rf code/*.hpp ../include/renderd7/
echo "Clean Assets..."
./clear_assets.sh
cd ..
#Clear Tools
echo "Clean Tools..."
cd tools
./clear_tools.sh
cd ..
#Finished
echo "Done!"