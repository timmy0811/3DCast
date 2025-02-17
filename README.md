This project is heavily under construction!
# 3DCast
A 3D Rendering Engine supporting rasterization as well as raytracing

## Build with CMake
This project uses CMake for project configuration. 
Make sure any submodules are cloned correctly (since they are a bit messed up). Also make sure to checkout the 'docking'-branch in the ImGui dependency.
Run the following steps to build the project:
```sh
git clone ... --recursive # To clone submodules!
cd ...
mkdir build
cd build
cmake ..
cmake --build .
```
