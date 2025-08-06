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
SCOP (which stands for "scope" - as in having a view or vision) is a comprehensive 3D object visualization application that can load, display, and interact with 3D objects stored in common file formats (.obj and .fdf). It was developed as an introduction to the OpenGL API, and as a crossover with the [FDF renderer](https://github.com/hugomgris/FDFPP) (a 2D map based wireframe visualizer). Beyond basic visualization, the project also implements a custom user interface (ImGui), post-processing shaders, and a complex .obj., .fdf and .mtl parsing pipeline. 

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

## Features

### Core Functionality
- **OBJ File Parsing**: Complete support for .obj files with materials (.mtl).
- **FDF File Parsing**: Complete support for .fdf files, translating 2D maps into vertex arrays.
- **OpenGL Rendering**: Modern OpenGL with programmable shaders.
- **Multiple Render Modes**: Solid, wireframe, line, and point visualization.
- **Material & Texture Support**: Full material parsing and texture loading.
- **Custom User Interface**: Complete user interface with model info, controls, and status and rendering tracking.
- **Interactive viewport**: Mouse controls for model interaction.
- **Real-time Toggles**: Switch between rendering modes, projections, filters and animations in real time.

### Rendering Modes
- **Solid Mode**: Full 3D rendering with lighting and textures.
- **Wireframe Mode**: Complete triangle edge visualization.
- **Line Mode**: Original FDF-style line rendering.
- **Vertex Mode**: Point cloud visualization.

## Controls

| Key | Action |
|-----|--------|
| `LMB` | Move the model around the viewport |
| `RMB` | Freely rotate the model around all its axes |
| `Scroll` | Zoom in/out |
| `V` | Toggle wireframe mode |
| `X` | Toggle vertex visualization |
| `P` | Toggle orthographic/perspective projection |
| `R` | Reset model to default position |
| `C` | Toggle CRT filter (post-processing) |
| `1` | Toggle auto-rotation |
| `Esc` | Exit application |


## In-depth Overview
As mentioned in the General Description, this project was developed as an introduction to the OpenGL API and as a practice environment for standard graphics rendering pipelines. After working on other rendering assignments based on the [MLX42 library](https://github.com/codam-coding-college/MLX42/blob/master/docs/42.md?utm_source=chatgpt.com), which can be defined as an OpenGL wrapper designed to work as a simplified API for entry level studen projects, the opportunity here is to traverse at a lower computational level, working directly with the core functionality and leaving behind the high abstraction of a second order library.  


## Execution Pipeline Structure

The SCOP rendering pipeline follows a systematic flow from file input to final frame display:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   File Input    │───▶│   File Parser   │───▶│  Data Validation│
│  (.obj/.fdf)    │    │  (OBJ/FDF/MTL)  │    │  & Processing   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                        │
┌─────────────────┐    ┌─────────────────┐              ▼
│  Final Display  │◀───│ Post-Processing │    ┌─────────────────┐
│   (Monitor)     │    │  (CRT Effects)  │◀───│  Mesh Creation  │
└─────────────────┘    └─────────────────┘    │ (VBO/VAO/EBO)   │
          ▲                        ▲          └─────────────────┘
          │                        │                    │
┌─────────────────┐    ┌─────────────────┐              ▼
│   UI Overlay    │    │   3D Rendering  │    ┌─────────────────┐
│   Rendering     │    │   Pipeline      │◀───│ Texture Loading │
└─────────────────┘    └─────────────────┘    │ & Material Prep │
          ▲                        ▲          └─────────────────┘
          │                        │                    │
          └────────┬───────────────┘                    ▼
                   │                           ┌─────────────────┐
             ┌─────────────────┐               │  Shader Program │
             │ Input Manager   │◀──────────────│   Compilation   │
             │ (Mouse/Keyboard)│               │   & Linking     │
             └─────────────────┘               └─────────────────┘
```


```

## Project Architecture

INPUT LAYER:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ .obj Files  │    │ .fdf Files  │    │ .mtl Files  │
│             │    │             │    │             │
│ • Vertices  │    │ • Height    │    │ • Textures  │
│ • Faces     │    │ • Grid      │    │ • Materials │
│ • Normals   │    │ • Colors    │    │ • Properties│
└─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │
       └───────────────────┼───────────────────┘
                           ▼
PROCESSING LAYER:
┌─────────────────────────────────────────────────────────┐
│                     Parser                              │
│                                                         │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐        │
│  │ OBJ Parser  │ │ FDF Parser  │ │ MTL Parser  │        │
│  │             │ │             │ │             │        │
│  │ • Tokenize  │ │ • Grid→3D   │ │ • Materials │        │
│  │ • Validate  │ │ • Heights   │ │ • Textures  │        │
│  │ • Convert   │ │ • Convert   │ │ • Properties│        │
│  └─────────────┘ └─────────────┘ └─────────────┘        │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
GRAPHICS LAYER:
┌─────────────────────────────────────────────────────────┐
│                   GPU Pipeline                          │
│                                                         │
│ ┌─────────────┐    ┌─────────────┐    ┌─────────────┐   │
│ │   Vertex    │───▶│  Rasterizer │───▶│  Fragment   │   │
│ │   Shader    │    │             │    │   Shader    │   │
│ │             │    │ • Triangles │    │             │   │
│ │ • Transform │    │ • Clipping  │    │ • Lighting  │   │
│ │ • Project   │    │ • Culling   │    │ • Texturing │   │
│ │ • Lighting  │    │             │    │ • Effects   │   │
│ └─────────────┘    └─────────────┘    └─────────────┘   │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
OUTPUT LAYER:
┌─────────────────────────────────────────────────────────┐
│                 Frame Composition                       │
│                                                         │
│ ┌─────────────┐    ┌─────────────┐    ┌─────────────┐   │
│ │ 3D Geometry │    │Post Process │    │ UI Overlay  │   │
│ │             │───▶│             │───▶│             │   │
│ │ • Models    │    │ • CRT FX    │    │ • Panels    │   │
│ │ • Lighting  │    │ • Filters   │    │ • Controls  │   │
│ │ • Textures  │    │ • Effects   │    │ • Stats     │   │
│ └─────────────┘    └─────────────┘    └─────────────┘   │
└─────────────────────────────────────────────────────────┘
```

