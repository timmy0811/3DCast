# 3DCast

![Status](https://img.shields.io/badge/status-under%20development-yellow)

![Main Preview](https://github.com/timmy0811/3DCast/blob/Projects/img/prev4.jpg)


A 3D Rendering Engine supporting both rasterization and raytracing.

> ⚠️ This project is heavily under construction!

## Features (by now)

- Scene management
- Entity Component system
- Rasterizer Viewport
- Skyboxes with environment light calculation
- Different Light types
- PBR Materials
- Loading and Transforming of Models
- Cross-platform support (Windows, Linux)
- Editor themes
- Editable Keymap
- Debug Console and diagnostics
- Endless orientation grid (it's endless)
- Different Components for complex entities
- Dynamic gizmos for translation, scaling, and rotating

## Under the Hood

- Graphics API abstraction
- Massively scalable Memory manager
- Fused deferred and forward shading

![Features Highlight](https://github.com/timmy0811/3DCast/blob/Projects/img/feature1.jpg)

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

