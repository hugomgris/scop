# <h1 align="center">SCOP</h1>

<p align="center">
   <b>A 3D object viewer built with OpenGL in C++, featuring modular rendering, multiple display modes, multi-texture and multi-shader handling, and real-time interaction controls. </b><br>
</p>

---

<p align="center">
    <img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/hugomgris/FDFPP?color=lightblue" />
    <img alt="Code language count" src="https://img.shields.io/github/languages/count/hugomgris/FDFPP?color=yellow" />
    <img alt="GitHub top language" src="https://img.shields.io/github/languages/top/hugomgris/FDFPP?color=blue" />
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/hugomgris/FDFPP?color=green" />
    <img alt="GitHub issues" src="https://img.shields.io/github/issues/hugomgris/FDFPP?color=orange" />
    <img alt="GitHub forks" src="https://img.shields.io/github/forks/hugomgris/FDFPP?color=purple" />
    <img alt="GitHub stars" src="https://img.shields.io/github/stars/hugomgris/FDFPP?color=gold" />
    <img alt="GitHub license" src="https://img.shields.io/github/license/hugomgris/FDFPP?color=gray" />
</p>

## General Description
SCOP (which stands for "scope" - as in having a view or vision) is a comprehensive 3D object visualization application that can load, display, and interact with 3D objects stored in common file formats (.obj and .fdf). It was developed as an introduction to the OpenGL API, and as a crossover with the FDF renderer (a 2D map based wireframe visualizer). Beyond basic visualization, the project also implements a custom user interface (ImGui), post-processing shaders, and a complex .obj., .fdf and .mtl parsing pipeline. 

## Preview

![Main Project Screenshot](img/screenshot_01.png)
![Main Project Screenshot](img/screenshot_02.png)
![Main Project Screenshot](img/screenshot_03.png)

## Build & Execution Instructions

### Prerequisites
- **C++ Compiler**: g++ or clang++ with C++17 support or higher
- **OpenGL**: Version 3.3 (CORE) or higher
- **GLFW3**: Graphics library for window management and input
- **Git**: For cloning dependencies (GLM and ImGui)

### Installation Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/hugomgris/scop.git
   cd scop
   ```

2. **Install system dependencies:**
   
   **On Ubuntu/Debian:**
   ```bash
   sudo apt update
   sudo apt install build-essential libglfw3-dev libgl1-mesa-dev pkg-config git
   ```
   
   **On macOS (with Homebrew):**
   ```bash
   brew install glfw pkg-config git
   ```
   
   **On Fedora/RHEL:**
   ```bash
   sudo dnf install gcc-c++ glfw-devel mesa-libGL-devel pkg-config git
   ```

3. **Check dependencies (optional):**
   ```bash
   make check-deps
   ```

4. **Build the project:**
   ```bash
   make
   ```
   
   *Note: The build process automatically downloads GLM and ImGui libraries if not present.*

5. **Run the application:**
   ```bash
   ./scop <path_to_obj_file>
   ```

### Example Usage
```bash
# Run with sample 3D models
./scop resources/objects/42.obj
./scop resources/objects/teapot.obj
./scop resources/objects/cube.obj

# List all available models
make list-models

# For models with textures
./scop resources/objects/ninja/Ninja.obj.obj
./scop resources/objects/mariohead.obj
```

## Detailed Features