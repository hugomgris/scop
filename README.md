# SCOP - 3D Object Renderer

🚀 A comprehensive 3D object viewer built with OpenGL in C++ for the 42 School `scop` project.  
🎮 Features modular rendering, multiple display modes, and real-time interaction controls.

---

## 🎯 Features

### Core Functionality
- **OBJ File Parsing**: Complete support for .obj files with materials (.mtl)
- **OpenGL Rendering**: Modern OpenGL with programmable shaders
- **Multiple Render Modes**: Solid, wireframe, line, and point visualization
- **Material & Texture Support**: Full material parsing and texture loading
- **Interactive Camera**: Mouse and keyboard controls for navigation
- **Real-time Toggles**: Switch between rendering modes on-the-fly

### Rendering Modes
- **Solid Mode**: Full 3D rendering with lighting and textures
- **Wireframe Mode**: Complete triangle edge visualization
- **Line Mode**: Original FDF-style line rendering
- **Vertex Mode**: Point cloud visualization

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W/A/S/D` | Camera movement |
| `Space/Ctrl` | Camera up/down |
| `Q/E` | Camera roll |
| `Mouse` | Look around (hold left click) |
| `Scroll` | Zoom in/out |
| `P` | Toggle wireframe mode |
| `V` | Toggle vertex visualization |
| `1` | Toggle orthographic/perspective projection |
| `R` | Reset camera to default position |
| `X` | Toggle auto-rotation |
| `Esc` | Exit application |

---

## 🔧 Wireframe Implementation

### Technical Overview

The wireframe rendering system uses dedicated line indices rather than OpenGL's `glPolygonMode` for complete control over edge visualization.

### Architecture

```cpp
class Mesh {
private:
    unsigned int _wireframeIBO;           // Dedicated wireframe index buffer
    std::vector<unsigned int> _wireframeIndices;  // Line indices
    int _wireframeIndexCount;             // Number of wireframe indices

public:
    void generateWireframeIndices();      // Convert triangles to lines
    unsigned int getWireframeIBO() const; // Access wireframe buffer
    int getWireframeIndexCount() const;   // Get wireframe index count
};
```

### Algorithm: Triangle to Line Conversion

```cpp
void Mesh::generateWireframeIndices() {
    const std::vector<unsigned int>& triangleIndices = _parser->getIndices();
    _wireframeIndices.clear();
    
    // Convert each triangle to 3 lines
    for (size_t i = 0; i < triangleIndices.size(); i += 3) {
        if (i + 2 < triangleIndices.size()) {
            unsigned int v0 = triangleIndices[i];
            unsigned int v1 = triangleIndices[i + 1];
            unsigned int v2 = triangleIndices[i + 2];
            
            // Add the three edges: v0-v1, v1-v2, v2-v0
            _wireframeIndices.push_back(v0); _wireframeIndices.push_back(v1);
            _wireframeIndices.push_back(v1); _wireframeIndices.push_back(v2);
            _wireframeIndices.push_back(v2); _wireframeIndices.push_back(v0);
        }
    }
    _wireframeIndexCount = _wireframeIndices.size();
}
```

### Rendering Pipeline

1. **Buffer Creation**: Generate separate IBO for wireframe indices
2. **Index Generation**: Convert triangle indices to line pairs
3. **Shader Setup**: Configure line-specific uniforms
4. **Rendering**: Use `GL_LINES` primitive with wireframe IBO

### Advantages Over glPolygonMode

| Aspect | glPolygonMode | Dedicated Line Indices |
|--------|---------------|----------------------|
| **Edge Coverage** | Incomplete (shared edges missing) | Complete (all triangle edges) |
| **Color Control** | Limited | Full control via shaders |
| **Performance** | GPU polygon mode switch | Optimized line rendering |
| **Flexibility** | Fixed behavior | Customizable line generation |

### Shader Integration

```glsl
// Fragment shader wireframe handling
if (u_isLineMode) {
    FragColor = vec4(u_lineColor, 1.0);  // Consistent OFF_WHITE color
    return;
}
```

### Material-Aware Wireframe

The system handles materials intelligently:
- **Wireframe Mode**: Renders complete mesh wireframe once (ignores material groups)
- **Normal Mode**: Respects material boundaries and textures
- **Consistent Coloring**: All wireframe lines use OFF_WHITE regardless of underlying materials

---

## 🏗️ Build Instructions

### Prerequisites
- C++17 compatible compiler
- OpenGL 3.3+ support
- GLFW3
- GLAD (included)
- GLM (included)

### Building

```bash
# Clone the repository
git clone <repository-url>
cd scop

# Build with Make
make

# Run with an OBJ file
./scop resources/objects/42.obj
```

### File Structure

```
scop/
├── include/           # Header files
│   ├── App.hpp
│   ├── Mesh.hpp      # Core mesh handling
│   ├── Renderer.hpp  # Rendering engine
│   └── ...
├── src/
│   ├── app/          # Application logic
│   ├── renderer/     # Rendering implementation
│   ├── parser/       # OBJ/MTL parsing
│   └── utils/        # Utilities
├── resources/
│   ├── objects/      # Test OBJ files
│   ├── textures/     # Texture files
│   └── shaders/      # GLSL shaders
└── lib/              # External libraries (GLM)
```

---

## 🎨 Rendering Modes

### 1. Solid Rendering
- Full 3D visualization with Phong lighting
- Material-based texture mapping
- Ambient, diffuse, and specular lighting components

### 2. Wireframe Rendering
- **Implementation**: Dedicated line index generation
- **Coverage**: Every triangle edge rendered
- **Color**: Consistent OFF_WHITE lines
- **Performance**: Optimized `GL_LINES` rendering

### 3. Vertex Visualization
- Point cloud rendering with configurable point size
- Yellow vertex highlighting
- Combinable with other render modes

---

## � Project Roadmap

| Milestone | Status | Description |
|----------|--------|-------------|
| ✅ 0. Setup | Complete | GLFW + GLAD integration |
| ✅ 1. Mesh Loading | Complete | OBJ parsing and rendering |
| ✅ 2. Lighting | Complete | Phong lighting model |
| ✅ 3. Wireframe | Complete | Advanced wireframe rendering |
| ✅ 4. Textures | Complete | Material and texture support |
| 🔜 5. Optimization | In Progress | Performance improvements |

---

## � License

This project is part of the 42 School curriculum. Educational use only.

---

## 🤝 Contributing

This is an educational project for 42 School. While contributions are welcome for learning purposes, please respect the academic integrity guidelines of your institution.

