@echo off
setlocal enabledelayedexpansion

echo Starting 3DCast build process...

REM Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed or not in PATH! Please install CMake first.
    exit /b 1
)

REM Initialize all git submodules
echo Initializing git submodules...
git submodule update --init --recursive
if %ERRORLEVEL% NEQ 0 (
    echo Failed to initialize git submodules!
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build

REM Configure CMake
echo Configuring CMake...
cmake -B build -DCMAKE_BUILD_TYPE=Release -DVISUAL_STUDIO=ON
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build build --config Release --parallel %NUMBER_OF_PROCESSORS%
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build completed successfully!
echo Binaries can be found in: %CD%\build\bin\Release

pause
endlocal
