# 3DCast

![Status](https://img.shields.io/badge/status-under%20development-yellow)

A 3D Rendering Engine supporting both rasterization and raytracing.

> ⚠️ This project is heavily under construction!

## Features

- Modern OpenGL Rasterization Pipeline
- Ray Tracing capabilities
- Cross-platform support (Windows, Linux)
- ImGui-based user interface with docking support

## Dependencies

- GLFW for windowing
- GLEW for OpenGL extension loading
- ImGui for user interface
- Assimp for 3D model loading
- spdlog for logging
- yaml-cpp for configuration
- Native File Dialog for file operations

## Building the Project

### Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler
- Git for cloning the repository

### Build Instructions

#### Using the Build Scripts

**Windows:**

```batch
# Clone the repository with submodules
git clone https://github.com/yourusername/3DCast_Linux.git --recursive
cd 3DCast_Linux

# Run build script
install.bat
```

**Linux:**

```bash
# Clone the repository with submodules
git clone https://github.com/yourusername/3DCast_Linux.git --recursive
cd 3DCast_Linux

# For X11 (default)
./install.sh

# For Wayland
./install.sh --wayland
```

#### Manual Build

```bash
# Clone the repository with submodules
git clone https://github.com/yourusername/3DCast_Linux.git --recursive
cd 3DCast_Linux

# Build project
mkdir build
cd build
cmake ..
cmake --build . --config Release --parallel
```

## Directory Structure

```
3DCast_Linux/
├── 3DCast/            # Core engine library
├── 3DCast_Runtime/    # Application entry point
├── GLWrapper/         # OpenGL abstraction layer
├── Dependencies/      # Third-party libraries
├── resources/         # Assets and resources
├── CMakeLists.txt     # Main CMake configuration
├── install.bat        # Windows build script
└── install.sh         # Linux build script
```

## Running the Application

After building, the executable will be located in:

- Windows: `build/bin/Release/3DCast.exe`
- Linux: `build/bin/Release/3DCast`

Asset Files are copied automatically when using the install scripts. Alternatively copy the folder `3DCast/resources` and `3DCast_Runtime/imgui.ini` into the executables folder. 

