# ImGui Application

A starter C++ application using ImGui, GLFW, and CMake.

## Prerequisites

### macOS
```bash
brew install glfw3 cmake
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev cmake
```

### Windows
- Install [CMake](https://cmake.org/)
- Install [GLFW](https://www.glfw.org/download.html)

## Setup

1. Clone or navigate to the project directory
2. Download ImGui:
```bash
mkdir -p vendor
cd vendor
git clone https://github.com/ocornut/imgui.git
cd ..
```

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

```bash
./ImGuiApp
```

## Project Structure

- `CMakeLists.txt` - Build configuration
- `src/main.cpp` - Main application with ImGui setup
- `vendor/imgui/` - ImGui source code (downloaded separately)

## Features

- GLFW window management
- ImGui rendering with OpenGL 3.3 backend
- Demo ImGui windows with interactive controls
- Clear configuration for easy customization
