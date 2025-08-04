# MTL Material Support Implementation

## Overview

The project now supports MTL (Material Template Library) files for advanced material rendering. This allows for realistic lighting and material properties when rendering OBJ files.

## Features Implemented

### 1. MTL File Parsing
- **Automatic Detection**: MTL files are automatically loaded when referenced in OBJ files via `mtllib` directive
- **Material Properties**: Supports all standard MTL material properties:
  - `Ka` - Ambient color
  - `Kd` - Diffuse color
  - `Ks` - Specular color
  - `Ke` - Emission color
  - `Ns` - Specular exponent (shininess)
  - `d` / `Tr` - Transparency
  - `Ni` - Index of refraction
  - `illum` - Illumination model
- **Texture Maps**: Supports texture map references:
  - `map_Kd` - Diffuse texture map
  - `map_Ka` - Ambient texture map
  - `map_Ks` - Specular texture map
  - `map_Bump` / `bump` - Normal map

### 2. Material Assignment
- **Per-Vertex Materials**: Each vertex can have its own material index
- **Dynamic Switching**: Materials are switched during OBJ parsing using `usemtl` directive
- **Multi-Material Objects**: Supports objects with multiple materials

### 3. Enhanced Rendering Pipeline
- **Shader Integration**: New shader (`3D_Materials.shader`) with material-aware rendering
- **Material Arrays**: Supports up to 16 different materials per object
- **Phong Lighting**: Proper Phong lighting model with material-specific properties

### 4. Material Information Display
- **Detailed Output**: Displays comprehensive material information at startup
- **Debug Information**: Shows material loading progress and assignment

## Usage

### Running with MTL Support
```bash
./scop path/to/object.obj
```

If the OBJ file references an MTL file, it will be automatically loaded and applied.

### Example Objects with Materials
- `resources/objects/cube.obj` - Simple single material
- `resources/objects/mariohead.obj` - Complex multi-material object (17 materials)

### Keyboard Controls
All existing controls remain the same:
- `ESC` - Exit
- `V` - Toggle vertex visualization
- `P` - Toggle projection mode
- `1` - Toggle wireframe mode
- `R` - Reset view
- Mouse + Left Click - Rotate camera
- Scroll - Zoom in/out

## Technical Implementation

### File Structure Changes
- **Types.hpp**: Added `Material` struct with all MTL properties
- **Parser.hpp/cpp**: Added MTL parsing functionality and material management
- **Renderer.hpp/cpp**: Added material uploading to shaders
- **Mesh.cpp**: Updated vertex attributes to include material index
- **Shaders**: New material-aware shader with proper lighting calculations

### Material Data Structure
```cpp
struct Material {
    std::string name;
    glm::vec3 ambient, diffuse, specular, emission;
    float shininess, transparency, refraction;
    int illumination;
    std::string diffuseMap, ambientMap, specularMap, normalMap;
};
```

### Shader Changes
- Added material uniform arrays supporting up to 16 materials
- Per-vertex material index attribute
- Enhanced fragment shader with material-specific lighting

## Sample Output

When loading an object with materials, you'll see output like:
```
Loading MTL file: resources/objects/mariohead.mtl
Loaded 17 materials from MTL file
Using material: Material_114_RED_png (index 10)
Using material: Material_118_BLUE_png (index 6)
...

=== Material Information ===
Total materials loaded: 18

Material 10: Material_114_RED_png
  Ambient:  (0, 0, 0)
  Diffuse:  (0.7969, 0, 0)
  Specular: (0, 0, 0)
  Emission: (0, 0, 0)
  Shininess: 0
  Transparency: 0
  Diffuse Map: resources/objects/RED.png
  Ambient Map: resources/objects/RED.png
===========================
```

## Performance Notes

- Materials are uploaded to GPU once at startup
- Supports up to 16 materials efficiently (can be extended if needed)
- Minimal performance impact on rendering loop
- Automatic fallback to default material for objects without MTL files

## Future Enhancements

Potential improvements for the material system:
1. **Texture Loading**: Actually load and bind texture images
2. **Normal Mapping**: Implement normal map support in shaders
3. **PBR Materials**: Add support for physically-based rendering
4. **Material Hot-Reloading**: Real-time material editing and reloading
5. **Material Editor**: GUI for editing material properties
