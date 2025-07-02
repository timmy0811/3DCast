#!/bin/bash

echo "Starting 3DCast build process..."

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "CMake is not installed! Please install CMake first."
    exit 1
fi

# Check for Wayland flag
USE_WAYLAND=""
if [ "$1" == "--wayland" ]; then
    echo "Using Wayland display server..."
    USE_WAYLAND="-DWAYLAND=ON"
fi

# Initialize all git submodules
echo "Initializing git submodules..."
git submodule update --init --recursive
if [ $? -ne 0 ]; then
    echo "Failed to initialize git submodules!"
    exit 1
fi

# Create build directory
mkdir -p build

# Configure CMake
echo "Configuring CMake..."
cmake -B build -DCMAKE_BUILD_TYPE=Release $USE_WAYLAND
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "Building project..."
# Use number of CPU cores for parallel build
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
cmake --build build --config Release --parallel $CORES
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo
echo "Build completed successfully!"
echo "Binaries can be found in: $(pwd)/build/bin/Release"
