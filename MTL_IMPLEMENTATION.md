# MTL File Handling and Multi-Texture Implementation

## Overview

This document provides a comprehensive guide to the MTL (Material Template Library) file handling and multi-texture rendering implementation added to the SCOP project. The implementation transforms a basic OpenGL renderer with hardcoded texture paths into a sophisticated material-aware rendering system capable of handling complex models with multiple materials and textures.

## Table of Contents

1. [Project Context](#project-context)
2. [Initial State](#initial-state)
3. [Implementation Goals](#implementation-goals)
4. [Phase 1: Basic MTL Parsing](#phase-1-basic-mtl-parsing)
5. [Phase 2: Texture Management System](#phase-2-texture-management-system)
6. [Phase 3: Multi-Material Support](#phase-3-multi-material-support)
7. [Phase 4: Advanced Rendering Pipeline](#phase-4-advanced-rendering-pipeline)
8. [Technical Architecture](#technical-architecture)
9. [Usage Examples](#usage-examples)
10. [Performance Considerations](#performance-considerations)
11. [Future Enhancements](#future-enhancements)

## Project Context

**SCOP** is a 3D model viewer and renderer developed as part of the 42 School curriculum. The project focuses on OpenGL rendering, 3D mathematics, and computer graphics fundamentals. The initial implementation supported basic OBJ file loading with hardcoded texture paths.

### Technologies Used
- **Language**: C++17
- **Graphics API**: OpenGL 3.3 Core Profile
- **Math Library**: GLM (OpenGL Mathematics)
- **Image Loading**: stb_image
- **Window Management**: GLFW
- **Build System**: Custom Makefile

## Initial State

Before the MTL implementation, the system had:

### Existing Features
- Basic OBJ file parsing (vertices, texture coordinates, normals, faces)
- Single hardcoded texture loading (`resources/textures/Unicorns.png`)
- Simple vertex/fragment shader pipeline
- Basic camera controls and projection modes
- FDF (heightmap) file support

### Limitations
- **Hardcoded Texture Path**: All models used the same texture regardless of material definitions
- **No Material Support**: MTL files were completely ignored
- **Single Texture Per Model**: No way to render different parts of a model with different textures
- **Limited Realism**: Models couldn't display their intended appearance

### Example of Initial Texture Loading
```cpp
// Old approach - hardcoded texture
Texture texture("resources/textures/Unicorns.png");
texture.Bind();
```

## Implementation Goals

The MTL implementation aimed to achieve:

1. **Full MTL File Support**: Parse and understand material definitions
2. **Multi-Texture Rendering**: Support models with multiple materials/textures
3. **Intelligent Texture Management**: Efficient loading and caching system
4. **Backward Compatibility**: Maintain support for existing single-material models
5. **Professional Quality**: Industry-standard material handling

## Phase 1: Basic MTL Parsing

### 1.1 Material Data Structure Enhancement

**File**: `include/Types.hpp`

Enhanced the existing `Material` struct to support complete MTL specifications:

```cpp
struct Material {
    std::string name;
    
    // Colors
    glm::vec3 ambient = glm::vec3(0.2f);     // Ka
    glm::vec3 diffuse = glm::vec3(0.8f);     // Kd  
    glm::vec3 specular = glm::vec3(0.0f);    // Ks
    glm::vec3 emission = glm::vec3(0.0f);    // Ke
    
    // Material properties
    float shininess = 0.0f;                  // Ns
    float opacity = 1.0f;                    // d or Tr
    float refractiveIndex = 1.0f;            // Ni
    int illuminationModel = 2;               // illum
    
    // Texture maps
    std::string diffuseMap;                  // map_Kd
    std::string normalMap;                   // map_Bump or bump
    std::string specularMap;                 // map_Ks
    std::string ambientMap;                  // map_Ka
    std::string opacityMap;                  // map_d
    std::string displacementMap;             // disp
};
```

### 1.2 Parser Enhancement

**Files**: `include/Parser.hpp`, `src/parser/Parser.cpp`

Added MTL parsing capabilities to the existing OBJ parser:

```cpp
// New methods added to Parser class
void parseMTL(const std::string &filePath);
const std::vector<Material> &getMaterials() const;
const Material* getMaterialByName(const std::string& name) const;
```

### 1.3 MTL File Format Support

The `parseMTL` method supports standard MTL directives:

- **Material Definition**: `newmtl <name>`
- **Color Properties**: `Ka`, `Kd`, `Ks`, `Ke` (ambient, diffuse, specular, emission)
- **Material Properties**: `Ns` (shininess), `d`/`Tr` (opacity), `Ni` (refractive index), `illum`
- **Texture Maps**: `map_Kd`, `map_Ka`, `map_Ks`, `map_Bump`, `map_d`, `disp`

### 1.4 OBJ Integration

Enhanced OBJ parsing to handle material references:

```cpp
// In parseOBJ method
if (type == "mtllib") {
    std::string mtlFileName;
    iss >> mtlFileName;
    
    // Get the directory of the OBJ file
    std::string objDir = filePath.substr(0, filePath.find_last_of("/\\"));
    std::string mtlPath = objDir + "/" + mtlFileName;
    
    // Parse the MTL file
    parseMTL(mtlPath);
}
```

## Phase 2: Texture Management System

### 2.1 TextureLoader Class Design

**Files**: `include/TextureLoader.hpp`, `src/renderer/TextureLoader.cpp`

Created a sophisticated texture management system:

```cpp
class TextureLoader {
public:
    // Load a texture from file path
    std::shared_ptr<Texture> loadTexture(const std::string& filePath);
    
    // Load textures for a material
    std::shared_ptr<Texture> loadMaterialTextures(const Material& material);
    
    // Load all materials' textures
    std::unordered_map<int, std::shared_ptr<Texture>> loadAllMaterialTextures(
        const std::vector<Material>& materials);
    
    // Texture validation and caching
    bool isValidTexturePath(const std::string& filePath) const;
    std::shared_ptr<Texture> getTexture(const std::string& filePath);

private:
    // Texture cache for efficiency
    std::unordered_map<std::string, std::shared_ptr<Texture>> _textureCache;
};
```

### 2.2 Key Features

**Texture Caching**: Prevents loading the same texture multiple times
```cpp
// Check cache before loading
auto it = _textureCache.find(filePath);
if (it != _textureCache.end()) {
    return it->second;
}
```

**Path Resolution**: Automatically resolves relative texture paths
```cpp
// Get MTL directory for relative texture paths
std::string mtlDir = filePath.substr(0, filePath.find_last_of("/\\"));
currentMaterial->diffuseMap = mtlDir + "/" + texturePath;
```

**Format Support**: Supports multiple image formats (PNG, JPG, BMP, TGA, etc.)
```cpp
bool isSupportedFormat(const std::string& filePath) const {
    const std::vector<std::string> supportedExtensions = {
        ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif", ".psd", ".hdr", ".pic"
    };
    // Check if file extension is supported
}
```

### 2.3 Error Handling and Fallbacks

**Graceful Degradation**: System continues to work even when textures are missing
```cpp
try {
    auto texture = std::make_shared<Texture>(filePath);
    _textureCache[filePath] = texture;
    return texture;
} catch (const std::exception& e) {
    std::cerr << "Error loading texture " << filePath << ": " << e.what() << std::endl;
    return nullptr;
}
```

## Phase 3: Multi-Material Support

### 3.1 Material Group System

**File**: `include/Types.hpp`

Added data structures to track materials per face:

```cpp
struct MaterialGroup {
    std::string materialName;
    int materialIndex;
    std::vector<unsigned int> indices;  // Indices for this material group
};

struct Face {
    unsigned int indices[3];  // Triangle indices
    int materialIndex;        // Index into materials array (-1 = no material)
};
```

### 3.2 Material Tracking During OBJ Parsing

**File**: `src/parser/Parser.cpp`

Enhanced face parsing to track which material each triangle belongs to:

```cpp
// Track current material during parsing
std::string _currentMaterial;
int _currentMaterialIndex;
std::vector<MaterialGroup> _materialGroups;

// When encountering usemtl directive
if (type == "usemtl") {
    std::string materialName;
    iss >> materialName;
    
    _currentMaterial = materialName;
    _currentMaterialIndex = getMaterialIndex(materialName);
    
    // Create new material group if needed
    // ... material group management
}
```

### 3.3 Face-to-Material Association

During face parsing, each triangle is associated with the current material:

```cpp
// After processing a triangle
if (_currentMaterialIndex >= 0 && !_materialGroups.empty()) {
    for (auto& group : _materialGroups) {
        if (group.materialName == _currentMaterial) {
            group.indices.push_back(triangleIndices[0]);
            group.indices.push_back(triangleIndices[1]);
            group.indices.push_back(triangleIndices[2]);
            break;
        }
    }
}
```

## Phase 4: Advanced Rendering Pipeline

### 4.1 Multi-Material Rendering Strategy

**File**: `src/app/App.cpp`

Implemented intelligent rendering that switches between single and multi-material modes:

```cpp
void App::run() {
    // Load all material textures
    const auto& materials = _parser->getMaterials();
    
    if (!materials.empty()) {
        _materialTextures = _textureLoader->loadAllMaterialTextures(materials);
        std::cout << "Using multi-material rendering with " 
                  << _materialTextures.size() << " textures" << std::endl;
    }
    
    // Fallback to single texture if no materials found
    if (!_currentTexture) {
        _currentTexture = _textureLoader->loadTexture("resources/textures/Unicorns.png");
    }
    
    // Rendering loop with material detection
    if (!materialGroups.empty() && !_materialTextures.empty()) {
        renderWithMaterials();  // Multi-material pipeline
    } else {
        // Traditional single-texture rendering
        _renderer->draw(*_mesh, _mode, _inputManager->getCameraPosition(), _showVertices);
    }
}
```

### 4.2 Multi-Material Rendering Implementation

The `renderWithMaterials()` method implements per-material-group rendering:

```cpp
void App::renderWithMaterials() {
    const auto& materialGroups = _parser->getMaterialGroups();
    
    // Clear screen once
    setClearColor(Colors::BLACK_CHARCOAL_1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render each material group separately
    for (const auto& group : materialGroups) {
        if (group.indices.empty()) continue;
        
        // Bind appropriate texture for this material
        auto textureIt = _materialTextures.find(group.materialIndex);
        if (textureIt != _materialTextures.end()) {
            textureIt->second->Bind(0);
        }
        
        // Create temporary index buffer for this material group
        unsigned int materialIBO;
        glGenBuffers(1, &materialIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, materialIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     group.indices.size() * sizeof(unsigned int), 
                     group.indices.data(), 
                     GL_DYNAMIC_DRAW);
        
        // Draw this material group
        GLCall(glBindVertexArray(_mesh->getVAO()));
        GLCall(glDrawElements(renderMode, static_cast<GLsizei>(group.indices.size()), 
                             GL_UNSIGNED_INT, nullptr));
        
        // Cleanup temporary buffer
        glDeleteBuffers(1, &materialIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->getIBO());
    }
}
```

## Technical Architecture

### Data Flow

```
OBJ File → Parser → Material Groups → TextureLoader → Multi-Material Renderer
    ↓           ↓            ↓              ↓                ↓
MTL File → Materials → Face Mapping → Texture Cache → Per-Group Rendering
```

### Class Relationships

```
App
├── Parser (enhanced with MTL support)
│   ├── Materials vector
│   └── MaterialGroups vector
├── TextureLoader (new)
│   ├── Texture cache
│   └── Material texture mapping
├── Renderer (enhanced)
└── Mesh (unchanged)
```

### Memory Management

- **Smart Pointers**: `std::shared_ptr<Texture>` for automatic memory management
- **Texture Caching**: Prevents memory waste from duplicate texture loading
- **Temporary Buffers**: Dynamic IBO creation for material groups with proper cleanup

## Usage Examples

### Example 1: Simple Model (42.obj)

```bash
./scop resources/objects/42.obj
```

**Output**:
```
Found material: Material
Parsed 1 materials from resources/objects/42.mtl
No valid textures found for material: Material
No material textures found, using fallback texture
Loaded texture: resources/textures/Unicorns.png
```

**Result**: Single-texture rendering with fallback texture

### Example 2: Complex Model (mariohead.obj)

```bash
./scop resources/objects/mariohead.obj
```

**Output**:
```
Found material: Material_123_BLACK_png
Found diffuse texture: resources/objects/../textures/BLACK.png
[... 16 more materials ...]
Parsed 17 materials from resources/objects/mariohead.mtl
Loaded 17 textures from 17 materials
Using multi-material rendering with 17 textures
```

**Result**: Multi-material rendering with proper texture mapping

### Example 3: Creating Custom Materials

**my_model.mtl**:
```mtl
newmtl Wood
Kd 0.8 0.6 0.4
map_Kd textures/wood.png

newmtl Metal
Kd 0.7 0.7 0.8
Ns 200.0
map_Kd textures/metal.png
```

**my_model.obj**:
```obj
mtllib my_model.mtl
# vertices and faces...
usemtl Wood
f 1 2 3
usemtl Metal
f 4 5 6
```

## Performance Considerations

### Optimizations Implemented

1. **Texture Caching**: Identical textures are loaded once and reused
2. **Material Grouping**: Faces are batched by material to minimize state changes
3. **Smart Rendering**: Automatically chooses between single and multi-material pipelines
4. **Memory Management**: Temporary buffers are created and destroyed efficiently

### Performance Metrics

- **Texture Loading**: O(1) for cached textures, O(n) for unique textures
- **Rendering**: O(m) where m = number of material groups (typically much less than face count)
- **Memory Usage**: Minimal overhead with smart pointer management

### Scalability

The system scales well with:
- **Large Models**: Material grouping reduces draw calls
- **Multiple Textures**: Caching prevents redundant loading
- **Complex Scenes**: Efficient state management

## Future Enhancements

### Potential Improvements

1. **Advanced Material Properties**
   - Implement specular, ambient, and emission rendering
   - Support for normal mapping and bump mapping
   - Transparency and alpha blending

2. **Shader Enhancements**
   - Material-specific shader programs
   - Phong/Blinn-Phong lighting models
   - PBR (Physically Based Rendering) support

3. **Performance Optimizations**
   - Vertex buffer optimization for material groups
   - Instanced rendering for repeated materials
   - Level-of-detail (LOD) system

4. **Extended Format Support**
   - Additional texture types (cubemaps, 3D textures)
   - HDR texture support
   - Compressed texture formats

### Implementation Roadmap

1. **Short Term**: Material property utilization in shaders
2. **Medium Term**: Advanced lighting and normal mapping
3. **Long Term**: PBR pipeline and advanced effects

## Conclusion

The MTL file handling and multi-texture implementation represents a significant enhancement to the SCOP project, transforming it from a basic model viewer into a sophisticated rendering system. The implementation demonstrates:

- **Professional-grade architecture** with proper separation of concerns
- **Robust error handling** with graceful fallbacks
- **Performance optimization** through caching and efficient rendering
- **Extensible design** that supports future enhancements

This foundation enables the rendering of complex, realistic 3D models while maintaining compatibility with simple single-material objects, making the SCOP renderer suitable for a wide range of 3D content.
