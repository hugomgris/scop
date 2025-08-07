# <h1 align="center">SCOP</h1>

<p align="center">
   <b>A 3D object viewer built with OpenGL in C++, featuring modular rendering, multiple display modes, multi-texture and multi-shader handling, and real-time interaction controls. </b><br>
</p>

---

<p align="center">
    <img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/hugomgris/scop?color=lightblue" />
    <img alt="Code language count" src="https://img.shields.io/github/languages/count/hugomgris/scop?color=yellow" />
    <img alt="GitHub top language" src="https://img.shields.io/github/languages/top/hugomgris/scop?color=blue" />
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/hugomgris/scop?color=green" />
    <img alt="GitHub issues" src="https://img.shields.io/github/issues/hugomgris/scop?color=orange" />
    <img alt="GitHub forks" src="https://img.shields.io/github/forks/hugomgris/scop?color=purple" />
    <img alt="GitHub stars" src="https://img.shields.io/github/stars/hugomgris/scop?color=gold" />
    <img alt="GitHub license" src="https://img.shields.io/github/license/hugomgris/scop?color=gray" />
</p>

## Table of Contents

- [General Description](#general-description)
- [Preview](#preview)
- [Build & Execution Instructions](#build--execution-instructions)
  - [Prerequisites](#prerequisites)
  - [Installation Steps](#installation-steps)
  - [Example Usage](#example-usage)
- [Features](#features)
  - [Core Functionality](#core-functionality)
  - [Rendering Modes](#rendering-modes)
- [Controls](#controls)
- [In-Depth Overview](#in-depth-overview)
  - [File Parsing](#file-parsing)
  - [OpenGL Buffer Management](#opengl-buffer-management-sending-data-to-the-gpu)
  - [Advanced Wireframe Implementation](#advanced-wireframe-implementation)
  - [Shader Programming](#shader-programming-talking-with-the-gpu-and-understanding-each-other)
  - [Multi-Texture Material System](#multi-texture-material-system)
  - [UI Integration](#ui-integration-️)
  - [Interactive Model Manipulation](#interactive-model-manipulation)
  - [Post-Processing: CRT Effect](#post-processing-crt-effect)
  - [Mathematical Foundations](#mathematical-foundations)
  - [Performance Optimization Strategies](#performance-optimization-strategies)
- [Project Takeaways](#project-takeaways)
- [Execution Pipeline Structure](#execution-pipeline-structure)
- [Project Architecture](#project-architecture)
- [Troubleshooting](#troubleshooting)
- [Documentation](#documentation)
- [Performance Considerations](#performance-considerations)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

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


# In-Depth Overview

This project serves as an introduction to the OpenGL API and a practical implementation of standard graphics rendering pipelines. Unlike earlier assignments that utilized the [MLX42 library](https://github.com/codam-coding-college/MLX42/blob/master/docs/42.md), an OpenGL wrapper designed for simplified graphics APIs, this implementation operates directly at the OpenGL level, bypassing higher-level abstractions and enabling more precise control over rendering processes.

SCOP transforms plain 3D model data into rendered visuals. The system is structured into several core components, following the program’s flow from file parsing through rendering optimization and output.

## File Parsing

The application supports multiple 3D model formats, each requiring tailored parsing strategies:

### OBJ File Processing: Translating Raw Data Into Vertex Data

OBJ is a plain-text format used to describe 3D geometry through vertices, texture coordinates, normals, and face definitions. The parser reads these elements line by line:

```cpp
// Example OBJ content
v 0.0 0.0 0.0          // Vertex position
vt 0.0 0.0             // Texture coordinate  
vn 0.0 1.0 0.0         // Normal vector
f 1/1/1 2/2/2 3/3/3    // Face definition
```

Each prefix encodes specific geometry data:  
- `v` defines points in 3D space  
- `vt` encodes texture mapping coordinates  
- `vn` provides normal vectors for lighting calculations  
- `f` establishes the connectivity between vertices to form polygons

### MTL File Integration

MTL (Material Template Library) files define surface properties for use in rendering:

```cpp
// Material definition
newmtl Wood_Material
Kd 0.6 0.4 0.2        // Diffuse color
Ks 0.1 0.1 0.1        // Specular reflection
Ns 10.0               // Shininess
map_Kd wood.png       // Texture image
```

A single model can reference multiple materials. For instance, separate materials may define distinct textures and properties for walls, windows, and roofs within a single mesh.

### FDF File Support

The application also supports FDF (heightmap) files, which store 2D grid data that is transformed into 3D terrain meshes:

```cpp
// FDF grid becomes 3D vertices
0  1  2  3  4         
1  2  3  4  5    →    // Converted to 3D mesh with appropriate spacing
2  3  4  5  6         
```

## OpenGL Buffer Management: Sending Data to the GPU

Once parsed, geometry data must be transferred efficiently to GPU memory. OpenGL's basic pipeline is based on different data structures that need to be carefully set up before any attempt at rendering:

### Vertex Array Objects (VAO)

In the OpenGL graphics pipeline, a Vertex Array Object (VAO) acts as a container that stores the configuration state for how vertex data is interpreted during rendering. It keeps track of bound vertex buffers, attribute pointers, and index buffer bindings.

```cpp
glGenVertexArrays(1, &_VAO);
glBindVertexArray(_VAO);
```

By encapsulating the attribute layout and buffer associations, VAOs allow switching between different mesh configurations efficiently without reconfiguring vertex states each time.

### Vertex Buffer Objects (VBO)

A Vertex Buffer Object (VBO) stores raw vertex attribute data — such as positions, normals, texture coordinates — in GPU memory. This allows efficient access by the vertex shader during rendering.

```cpp
glGenBuffers(1, &_VBO);
glBindBuffer(GL_ARRAY_BUFFER, _VBO);
glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), 
             vertexData.data(), GL_STATIC_DRAW);
```

In the graphics pipeline, the VBO feeds per-vertex data to the input stage of the vertex shader. The `GL_STATIC_DRAW` usage hint signals that the data will not change over time, which enables OpenGL to optimize memory placement.

### Element Buffer Objects (EBO)

An Element Buffer Object (EBO), also known informally as an Index Buffer Object (IBO), stores indices that reference vertex data in the VBO. Instead of duplicating vertices for each triangle, an EBO allows multiple primitives to reuse shared vertex positions.

```cpp
unsigned int indices[] = {0, 1, 2};
```

By using indices, EBOs reduce memory usage and increase rendering efficiency. For example, a cube requires only 8 unique vertices but 36 indices (6 faces × 2 triangles × 3 vertices) to define all its faces. The EBO is bound to the VAO, ensuring correct index usage during draw calls like glDrawElements.

### Summary of Roles

| Component | Purpose                                  | Stage in Pipeline                     | Relationship         |
|----------|-------------------------------------------|---------------------------------------|----------------------|
| **VAO**  | Stores configuration and attribute layout | Input assembler                       | Binds VBO and EBO    |
| **VBO**  | Stores vertex data (positions, normals…)  | Input to vertex shader                | Bound via VAO        |
| **EBO**  | Stores indices for vertex reuse           | Primitive assembly (`glDrawElements`) | Bound via VAO        |

### The Simplest Way to Understand VAO/VBO/EBO Roles I Can Come Up With

`VBO` → A box of Lego bricks (what vertices ***are***) [The raw float values]

`VAO` → Blueprints for how to use those bricks (how to ***interpret*** the bytes in the VBO) [Structure of a single vertex]

`EBO` → Instructions on which bricks to use and in what order (indices to assemble triangles, lines, etc.) [How to ***use*** the vertices to form primitives]



## Advanced Wireframe Implementation

This project implements a custom wireframe system to sidestep the frustrating limitations of OpenGL’s native `glPolygonMode`.

### Why `glPolygonMode` Doesn’t Cut It

- Shared triangle edges get skipped  
- You can’t combine filled and wireframe rendering  
- You have practically no control over how things look  

In other words: if you're trying to render a wireframe visualization of your parsed model (especially anything more complex than a cube), you're going to lose edges, and it won't look right.

### A Custom Wireframe That *Actually Works*

Instead of relying on OpenGL's built-in outline mode, this implementation explicitly decomposes each triangle into three lines. Manual geometry, per edge:

```cpp
for (size_t i = 0; i < triangleIndices.size(); i += 3) {
    ...
}
```

This approach guarantees you *see* every edge, every time. You also gain control over line width, color, and general vibe — essential when you're aiming for something more stylized.

> It’s not just better — it’s *cooler*.  
> Toggle CRT and Wireframe mode on the `resources/objects/ninja/Ninja.obj` model, and be transported to a retro-futuristic realm of awesome.

![We're not tools of the Government, or any one else...— Gray Fox, Death's prisoner and undying shadow in the world of lights](img/screenshot_03.png)

---

## Shader Programming: Talking with the GPU and *Understanding Each Other*

Shaders are programs that run on the GPU. They’re the language you use to say:  
*"Hey, here's how I want this thing to look."*

They come in many forms, but the two main ones in this project are:

### Vertex Shaders

These deal with position and geometry — the layout of objects in 3D space. If your `.obj` file defines a cube, vertex shaders are what take that cube’s raw coordinates and place it correctly in the world, the camera view, and the screen:

```glsl
gl_Position = projection * view * model * vec4(aPos, 1.0);
```

They use transformation matrices passed from the CPU to the GPU via uniforms.

### Fragment Shaders

Fragment shaders are where the *style* lives. They define how every pixel inside those triangle faces is colored and lit — from simple flat shading to full-on texturing and lighting.

This project has two main fragment shaders:
- One for standard rendering (diffuse, ambient, specular, etc)
- One for CRT-style post-processing (lens distortion, chromatic aberration, scanlines, you name it)

```glsl
vec3 result = (ambient + diffuse) * texture(u_texture, TexCoord).rgb;
```

Lighting calculations use the [Phong](https://en.wikipedia.org/wiki/Phong_reflection_model) model for smooth results.

---

## Multi-Texture Material System

This SCOP implementation supports multiple materials per model, parsed from `.obj` and `.mtl` files. At runtime, the engine groups geometry by material, binds the correct texture, and draws each chunk accordingly:

```cpp
for (const auto &materialGroup : materialGroups) {
    ...
}
```

This reduces unnecessary texture swaps and makes Mario's head look like it’s 1997 again — specially with CRT filter ON.

![Mario's head model included exclusively for research purposes](img/screenshot_03.png)

## UI Integration 🖥️

The real-time UI interface, powered by ImGui, provides dynamic control and feedback. It is divided in two main frames, a left pannel with different render and performance related information, and a main window where the model is displayed.

### Renderer-State Synchronization

UI actions directly modify rendering parameters, and they offer an alternative way of manipulating the rendered view to the key callbacks. In less words: everything can be controlled by the mouse, but you can use keys too if that is more your jam:

```cpp
if (ImGui::Checkbox("Wireframe Mode [V]", &wireframeMode)) {
    ...
}
```

### Performance Monitoring

Frame times and FPS are tracked and displayed in real time. All credits for the FPS graphic go to ImGui (as it is due):

```cpp
_state.fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
```

## Interactive Model Manipulation

The input system allows intuitive control of 3D objects via mouse input (no keys here, sorry):

### 2D to 3D Translation

Mouse movement is converted to model transformations, both positon and rotation-wise:

```cpp
_modelOffset.x += xoffset * sensitivity;
```

Mouse interaction is constrained to the 3D viewport:

```cpp
if (xpos < _viewportBounds.x || xpos > _viewportBounds.z || 
    ypos < _viewportBounds.y || ypos > _viewportBounds.w) {
    return;
}
```

## Post-Processing: CRT Effect

A full post-processing pipeline applies stylized effects such as CRT emulation. Some may say that this type of effect is overused nowadays, but in my mind there's never too many scanlines — and chromatic aberrations are the best kind of, well, aberrations. A perfectly-lit Ninja  is fine. A distorted, glowy, rainbow-edged Ninja gets us into *art* territory.

### Framebuffer Usage

Instead of rendering directly to the screen, the scene is first drawn into an off-screen framebuffer — essentially a blank canvas that can be messed with before showing the result:

```cpp
postProcessor->bind();
renderer->drawScene();
postProcessor->render();
```

This allows for screen-wide effects to be applied in a single shader pass, rather than tweaking each model individually.

### Shader Effects

The CRT post-processing shader introduces a number of retro-inspired visual distortions:

-**Barrel distortion**, to simulate curved CRT screens.
-**Chromatic aberration**, for slight RGB channel separation.
-**Scanlines**, because I like them and I can't be stopped.

```glsl
float scanline = sin(coord.y * 800.0) * 0.04;
color.rgb -= scanline;
```

Bonus: it all runs on the GPU, so my obsession with CRT won’t tank performance.

## Mathematical Foundations

### Transformations

The model-view-projection (MVP) matrix transforms vertices from object space to screen space:

```cpp
mat4 mvp = projection * view * model;
```

### Lighting Calculations

Phong lighting is calculated using ambient, diffuse, and specular components:

```cpp
vec3 specular = Ks * pow(max(dot(reflectDir, viewDir), 0.0), shininess) * lightSpecular;
```

## Performance Optimization Strategies

Key performance strategies include:

- **Indexed rendering** to reduce memory use  
- **Material grouping** to minimize state changes  
- **GPU buffer usage** for faster memory access  
- **Depth testing and backface culling** to avoid unnecessary fragment processing:

```cpp
glEnable(GL_DEPTH_TEST);
glEnable(GL_CULL_FACE);
```

## Project Takeaways

This project demonstrates competence in:

1. OpenGL graphics pipeline implementation  
2. 3D mathematics and transformations  
3. Shader programming with GLSL  
4. Efficient file parsing (OBJ, MTL, FDF)  
5. Real-time UI integration  
6. Performance-oriented rendering  
7. Modular, maintainable software architecture


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

## Troubleshooting

### Common Issues

**Build Errors:**
- Ensure you have C++17 support: `gcc --version` or `clang --version`
- Check OpenGL support: `glxinfo | grep OpenGL` (Linux)

**Runtime Issues:**
- Models not loading: Verify file path and format
- Black screen: Check graphics drivers and OpenGL support
- Performance issues: Try simpler models first

**Dependency Issues:**
- Run `make check-deps` to verify all dependencies
- On older systems, you may need to install newer versions of GLFW3

## Documentation

- **README.md** - This file, project overview and setup
- **Code Documentation** - Generated with Doxygen (run `make docs`)

## Performance Considerations

- **Recommended Hardware**: Modern GPU with OpenGL 3.3+ support
- **Model Complexity**: Tested with models up to 100K vertices
- **Target Performance**: 60 FPS at 1920x1080 on mid-range hardware
- **Memory Usage**: Scales with model complexity and texture count

## License

This project is part of the 42 School Outer Core curriculum. Educational use only.

## Acknowledgments

- **42 School** for the project framework and learning environment
- **GLM** for mathematics library support
- **ImGui** for immediate mode GUI
- **stb_image** for texture loading
- **GLFW** for window management and input handling
