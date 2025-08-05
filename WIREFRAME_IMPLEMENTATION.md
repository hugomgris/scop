# Wireframe Implementation Technical Documentation

## Overview

This document provides a detailed technical breakdown of the wireframe rendering implementation in the SCOP project. The implementation uses dedicated line indices rather than OpenGL's `glPolygonMode` to achieve complete and consistent wireframe visualization.

## Problem Statement

### Issues with `glPolygonMode(GL_LINE)`

The standard OpenGL wireframe approach using `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` has several limitations:

1. **Incomplete Edge Coverage**: Shared edges between triangles are only rendered once
2. **Missing Internal Edges**: Some triangle edges may not be visible
3. **Limited Color Control**: Cannot easily apply consistent coloring
4. **Material Conflicts**: Texture colors bleed through wireframe lines

### Example Problem

```
Triangle A: v0 -> v1 -> v2
Triangle B: v1 -> v3 -> v2
```

With `glPolygonMode`, the shared edge `v1-v2` may not render consistently, and internal edges could be missing.

## Solution Architecture

### Core Components

```cpp
class Mesh {
private:
    unsigned int _wireframeIBO;                    // Dedicated wireframe index buffer
    std::vector<unsigned int> _wireframeIndices;   // Generated line indices
    int _wireframeIndexCount;                      // Number of wireframe indices

public:
    void generateWireframeIndices();               // Triangle-to-line conversion
    unsigned int getWireframeIBO() const;          // Access wireframe buffer
    int getWireframeIndexCount() const;            // Get wireframe count
};
```

### Algorithm Implementation

#### 1. Index Generation

```cpp
void Mesh::generateWireframeIndices() {
    const std::vector<unsigned int>& triangleIndices = _parser->getIndices();
    _wireframeIndices.clear();
    
    // Process triangles in groups of 3 vertices
    for (size_t i = 0; i < triangleIndices.size(); i += 3) {
        if (i + 2 < triangleIndices.size()) {
            unsigned int v0 = triangleIndices[i];
            unsigned int v1 = triangleIndices[i + 1];
            unsigned int v2 = triangleIndices[i + 2];
            
            // Generate three edges for each triangle
            // Edge 1: v0 -> v1
            _wireframeIndices.push_back(v0);
            _wireframeIndices.push_back(v1);
            
            // Edge 2: v1 -> v2
            _wireframeIndices.push_back(v1);
            _wireframeIndices.push_back(v2);
            
            // Edge 3: v2 -> v0
            _wireframeIndices.push_back(v2);
            _wireframeIndices.push_back(v0);
        }
    }
    
    _wireframeIndexCount = _wireframeIndices.size();
}
```

#### 2. Buffer Management

```cpp
void Mesh::bind() {
    // ... standard VAO/VBO setup ...
    
    // Create dedicated wireframe IBO
    glGenBuffers(1, &_wireframeIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _wireframeIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(unsigned int) * _wireframeIndices.size(), 
                 _wireframeIndices.data(), 
                 GL_STATIC_DRAW);
    
    // Restore original IBO binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
    
    // ... vertex attribute setup ...
}
```

#### 3. Rendering Pipeline

```cpp
// In Renderer::draw() for wireframe mode
if (wireframeMode) {
    // Set line-specific uniforms
    int lineColorLoc = glGetUniformLocation(_shader->getID(), "u_lineColor");
    int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
    
    setLineColor(lineColorLoc, Colors::OFF_WHITE);
    glLineWidth(2.0f);
    glUniform1i(isLineModeLoc, 1);
    
    // Bind wireframe-specific buffer and render
    GLCall(glBindVertexArray(mesh.getVAO()));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getWireframeIBO());
    GLCall(glDrawElements(GL_LINES, mesh.getWireframeIndexCount(), GL_UNSIGNED_INT, nullptr));
}
```

## Shader Integration

### Vertex Shader

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool u_isLineMode;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
```

### Fragment Shader

```glsl
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 u_lineColor;
uniform bool u_isLineMode;
// ... other uniforms ...

out vec4 FragColor;

void main()
{
    // Prioritize line mode rendering
    if (u_isLineMode) {
        FragColor = vec4(u_lineColor, 1.0);
        return;
    }
    
    // Standard lighting calculations for solid mode
    // ...
}
```

## Material Handling

### Challenge

Objects with multiple materials require special handling to ensure wireframe renders the complete mesh rather than individual material groups.

### Solution

```cpp
// In App::renderWithMaterials()
if (_wireframeMode) {
    // Set wireframe uniforms
    setLineColor(lineColorLoc, Colors::OFF_WHITE);
    glUniform1i(isLineModeLoc, 1);
    
    // Render complete wireframe once for entire mesh
    GLCall(glBindVertexArray(_mesh->getVAO()));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->getWireframeIBO());
    GLCall(glDrawElements(GL_LINES, _mesh->getWireframeIndexCount(), GL_UNSIGNED_INT, nullptr));
    
    // Break early - don't process individual material groups
    break;
}
```

## Performance Considerations

### Memory Usage

- **Triangle Mesh**: N triangles = 3N indices
- **Wireframe Mesh**: N triangles = 6N indices (2 indices per edge, 3 edges per triangle)
- **Memory Overhead**: 2x index memory for wireframe buffer

### Optimization Opportunities

1. **Edge Deduplication**: Remove duplicate edges to reduce memory usage
2. **Lazy Generation**: Generate wireframe indices only when needed
3. **LOD System**: Different wireframe detail levels based on distance

### Edge Deduplication Algorithm

```cpp
void Mesh::generateOptimizedWireframeIndices() {
    std::set<std::pair<unsigned int, unsigned int>> edgeSet;
    
    for (size_t i = 0; i < triangleIndices.size(); i += 3) {
        unsigned int v0 = triangleIndices[i];
        unsigned int v1 = triangleIndices[i + 1];
        unsigned int v2 = triangleIndices[i + 2];
        
        // Add edges (ensuring consistent ordering)
        edgeSet.insert({std::min(v0, v1), std::max(v0, v1)});
        edgeSet.insert({std::min(v1, v2), std::max(v1, v2)});
        edgeSet.insert({std::min(v2, v0), std::max(v2, v0)});
    }
    
    // Convert set to index vector
    _wireframeIndices.clear();
    for (const auto& edge : edgeSet) {
        _wireframeIndices.push_back(edge.first);
        _wireframeIndices.push_back(edge.second);
    }
}
```

## Testing and Validation

### Test Cases

1. **Simple Cube**: 6 faces, 12 triangles, 36 wireframe lines
2. **Complex Mesh**: Multiple materials, shared vertices
3. **Degenerate Cases**: Single triangles, overlapping vertices

### Validation Metrics

- **Edge Count**: Verify correct number of wireframe indices
- **Visual Completeness**: All triangle edges visible
- **Color Consistency**: Uniform OFF_WHITE coloring
- **Performance**: Frame rate comparison with/without wireframe

## Future Enhancements

### Advanced Features

1. **Adaptive Line Width**: Distance-based line thickness
2. **Edge Highlighting**: Silhouette edge detection
3. **Wireframe Transparency**: Blended wireframe over solid
4. **Animation Support**: Animated wireframe transitions

### Integration Possibilities

1. **Debug Visualization**: Normal vectors, tangent space
2. **Selection System**: Pick individual edges/triangles
3. **Editing Tools**: Interactive mesh modification
4. **Export Features**: Wireframe-only model export

## Conclusion

This wireframe implementation provides complete, consistent, and performant edge visualization for 3D meshes. By using dedicated line indices instead of OpenGL's polygon mode, we achieve:

- **100% Edge Coverage**: Every triangle edge is explicitly rendered
- **Consistent Coloring**: Uniform appearance regardless of materials
- **Flexible Control**: Easy integration with shader systems
- **Performance Optimization**: Efficient GPU rendering pipeline

The system successfully addresses the limitations of traditional wireframe approaches while maintaining clean architecture and extensibility for future enhancements.
