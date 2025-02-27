#!/bin/bash

echo "Starting 3DCast build process..."

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "CMake is not installed! Please install CMake first."
    exit 1
fi

# Create and enter build directory
mkdir -p build
cd build

# Initialize all git submodules
echo "Initializing git submodules..."
cd ..
git submodule update --init --recursive

# Configure CMake
echo "Configuring CMake..."
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo
echo "Build completed successfully!"
echo "Binaries can be found in: $(pwd)/bin/x64-Release"
cd .. 