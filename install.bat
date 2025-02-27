@echo off
setlocal enabledelayedexpansion

echo Starting 3DCast build process...

REM Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed or not in PATH! Please install CMake first.
    exit /b 1
)

REM Create and enter build directory
if not exist "build" mkdir build
cd build

REM Initialize all git submodules
echo Initializing git submodules...
cd ..
git submodule update --init --recursive

REM Configure CMake
echo Configuring CMake...
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo Building project...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build completed successfully!
echo Binaries can be found in: %CD%\bin\x64-Release
cd ..

endlocal
pause 