# UI Implementation Documentation

## Overview

The SCOP project features a comprehensive UI system built with Dear ImGui, providing real-time control and information display for the 3D renderer. The interface follows a professional two-panel design with synchronized keyboard shortcuts and comprehensive mesh visualization controls.

## Architecture

### Core Components

```cpp
class UIManager {
    UIState _state;      // Current UI state and values
    UILayout _layout;    // Window sizing and positioning
    GLFWwindow* _window; // GLFW window reference
    
    // Rendering methods
    void renderControlPanel();
    void renderMainViewport();
    
    // State management
    void updateMeshInfo(const Parser* parser);
    void updateCameraInfo(const InputManager* inputManager);
    void updatePerformanceStats(float deltaTime);
};
```

### State Management

```cpp
struct UIState {
    // Rendering modes
    bool wireframeMode = false;
    bool showVertices = false;
    bool orthographicProjection = false;
    bool autoRotation = false;
    
    // Camera controls
    float cameraSpeed = 2.5f;
    float rotationSpeed = 50.0f;
    glm::vec3 cameraPosition{0.0f};
    
    // Mesh information
    int vertexCount = 0;
    int indexCount = 0;
    int triangleCount = 0;
    int materialCount = 0;
    std::string currentFile = "";
    
    // Performance metrics
    float frameTime = 0.0f;
    float fps = 0.0f;
};
```

### Layout System

```cpp
struct UILayout {
    float leftPanelWidth = 350.0f;
    float windowWidth = 1920.0f;
    float windowHeight = 1080.0f;
    float panelPadding = 10.0f;
    ImVec2 renderAreaPos{};     // Dynamic render area position
    ImVec2 renderAreaSize{};    // Dynamic render area size
};
```

## UI Layout Design

### Two-Panel Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    SCOP Application Window                   │
├───────────────┬─────────────────────────────────────────────┤
│               │                                             │
│  Left Panel   │            Main Viewport                    │
│   (350px)     │         (Dynamic Size)                     │
│               │                                             │
│ ┌───────────┐ │ ┌─────────────────────────────────────────┐ │
│ │ Mesh Info │ │ │                                         │ │
│ └───────────┘ │ │         3D Rendering Area               │ │
│ ┌───────────┐ │ │      (OFF_WHITE Border)                 │ │
│ │ Controls  │ │ │                                         │ │
│ └───────────┘ │ │                                         │ │
│ ┌───────────┐ │ │                                         │ │
│ │ Camera    │ │ │                                         │ │
│ └───────────┘ │ │                                         │ │
│ ┌───────────┐ │ │                                         │ │
│ │ Stats     │ │ │                                         │ │
│ └───────────┘ │ └─────────────────────────────────────────┘ │
└───────────────┴─────────────────────────────────────────────┘
```

## UI Components

### 1. Control Panel (Left Side)

#### Mesh Information Section
```cpp
void UIManager::renderMeshInfo() {
    if (ImGui::CollapsingHeader("Mesh Information", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("File: %s", _state.currentFile.c_str());
        ImGui::Text("Vertices: %d", _state.vertexCount);
        ImGui::Text("Indices: %d", _state.indexCount);
        ImGui::Text("Triangles: %d", _state.triangleCount);
        ImGui::Text("Materials: %d", _state.materialCount);
    }
}
```

**Features:**
- Real-time file name display
- Vertex and index count statistics
- Triangle count calculation
- Material count from MTL files

#### Rendering Controls Section
```cpp
// Wireframe Mode Toggle [P]
if (ImGui::Checkbox("Wireframe Mode [P]", &wireframe)) {
    _state.wireframeMode = wireframe;
    if (onWireframeModeChanged) {
        onWireframeModeChanged(wireframe);
    }
}

// Vertex Visualization [V]
if (ImGui::Checkbox("Show Vertices [V]", &vertices)) {
    _state.showVertices = vertices;
    if (onVertexModeChanged) {
        onVertexModeChanged(vertices);
    }
}
```

**Controls Available:**
- **Wireframe Mode [P]**: Toggle between solid and wireframe rendering
- **Show Vertices [V]**: Display vertices as points
- **Orthographic [1]**: Switch projection modes
- **Auto Rotation [X]**: Enable automatic model rotation

#### Camera Controls Section
```cpp
ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
           _state.cameraPosition.x, 
           _state.cameraPosition.y, 
           _state.cameraPosition.z);

ImGui::SliderFloat("Camera Speed", &_state.cameraSpeed, 0.1f, 10.0f);
ImGui::SliderFloat("Rotation Speed", &_state.rotationSpeed, 1.0f, 100.0f);
```

**Features:**
- Real-time camera position display
- Adjustable movement speed (0.1 - 10.0)
- Adjustable rotation speed (1.0 - 100.0)
- Reset camera button [R]
- Keyboard shortcuts reference

#### Performance Statistics Section
```cpp
void UIManager::renderPerformanceStats() {
    ImGui::Text("FPS: %.1f", _state.fps);
    ImGui::Text("Frame Time: %.3f ms", _state.frameTime * 1000.0f);
    
    // Real-time FPS graph
    static float fps_history[100] = {};
    static int fps_history_offset = 0;
    fps_history[fps_history_offset] = _state.fps;
    fps_history_offset = (fps_history_offset + 1) % 100;
    
    ImGui::PlotLines("##FPS", fps_history, 100, fps_history_offset, 
                    nullptr, 0.0f, 120.0f, ImVec2(0, 80));
}
```

**Metrics:**
- Real-time FPS counter
- Frame time in milliseconds
- Visual FPS history graph (100 frame buffer)
- Performance trend visualization

### 2. Main Viewport (Right Side)

#### 3D Rendering Area
```cpp
void UIManager::renderMainViewport() {
    ImGui::SetNextWindowPos(_layout.renderAreaPos);
    ImGui::SetNextWindowSize(_layout.renderAreaSize);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                                   ImGuiWindowFlags_NoBackground; // Transparent for 3D rendering
    
    ImGui::Begin("3D Viewport", nullptr, window_flags);
    
    // Draw OFF_WHITE border
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(canvas_pos, 
                      ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), 
                      IM_COL32(230, 230, 230, 255), // OFF_WHITE
                      0.0f, 0, 2.0f);
    
    ImGui::End();
}
```

**Features:**
- Transparent background for OpenGL rendering
- OFF_WHITE border (230, 230, 230, 255)
- Dynamic sizing based on window dimensions
- Proper viewport coordinate mapping

## Integration System

### Callback Architecture

The UI uses a functional callback system for communication with the main application:

```cpp
// In App::setupUICallbacks()
_uiManager->onWireframeModeChanged = [this](bool wireframeMode) {
    this->handleWireframeToggle(wireframeMode);
};

_uiManager->onVertexModeChanged = [this](bool showVertices) {
    this->handleVertexToggle(showVertices);
};

_uiManager->onProjectionModeChanged = [this](bool useOrtho) {
    this->handleProjectionToggle(useOrtho);
};
```

### State Synchronization

```cpp
// Keyboard shortcuts update UI state
void App::handleWireframeToggle(bool wireframeMode) {
    _wireframeMode = wireframeMode;
    
    // Sync with UI
    if (_uiManager) {
        UIState currentState = _uiManager->getState();
        currentState.wireframeMode = wireframeMode;
        _uiManager->updateState(currentState);
    }
}
```

### Viewport Management

```cpp
// Dynamic aspect ratio calculation
const auto& layout = _uiManager->getLayout();
float renderAreaWidth = layout.renderAreaSize.x - 16.0f;  // Account for padding
float renderAreaHeight = layout.renderAreaSize.y - 16.0f;
float aspectRatio = renderAreaWidth / renderAreaHeight;

// Update rendering system
_inputManager->setAspectRatio(aspectRatio);

// Set OpenGL viewport
int viewportX = static_cast<int>(layout.renderAreaPos.x + windowPadding + borderSize);
int viewportY = static_cast<int>(layout.windowHeight - layout.renderAreaPos.y - layout.renderAreaSize.y + windowPadding + borderSize);
int viewportWidth = static_cast<int>(layout.renderAreaSize.x - (windowPadding + borderSize) * 2);
int viewportHeight = static_cast<int>(layout.renderAreaSize.y - (windowPadding + borderSize) * 2);

glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
```

## Rendering Pipeline Integration

### Frame Rendering Sequence

1. **UI Frame Start**
   ```cpp
   _uiManager->newFrame();  // Calculate layout, update window size
   ```

2. **State Updates**
   ```cpp
   _uiManager->updateMeshInfo(_parser);
   _uiManager->updateCameraInfo(_inputManager.get());
   _uiManager->updatePerformanceStats(_inputManager->getDeltaTime());
   ```

3. **3D Rendering Setup**
   ```cpp
   // Set viewport to render area
   glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
   
   // Reset OpenGL state
   glDisable(GL_SCISSOR_TEST);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   
   // Clear with background color
   setClearColor(Colors::BLACK_CHARCOAL_1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   ```

4. **3D Content Rendering**
   ```cpp
   _renderer->draw(*_mesh, _mode, _inputManager->getCameraPosition(), 
                   _showVertices, _wireframeMode);
   ```

5. **UI Overlay Rendering**
   ```cpp
   // Restore full viewport for UI
   glViewport(0, 0, windowWidth, windowHeight);
   
   // Render UI elements
   _uiManager->render();
   ```

6. **Frame Completion**
   ```cpp
   glfwSwapBuffers(_window);
   glfwPollEvents();
   ```

## Styling and Theming

### Color Scheme
```cpp
// Dark professional theme
ImVec4 _backgroundColor{0.1f, 0.1f, 0.1f, 1.0f};      // Dark background
ImVec4 _panelColor{0.15f, 0.15f, 0.15f, 1.0f};        // Panel background
ImVec4 _borderColor{0.9f, 0.9f, 0.9f, 1.0f};          // OFF_WHITE borders
ImVec4 _textColor{0.9f, 0.9f, 0.9f, 1.0f};            // Light text
ImVec4 _buttonColor{0.2f, 0.2f, 0.2f, 1.0f};          // Button background
ImVec4 _buttonHoveredColor{0.3f, 0.3f, 0.3f, 1.0f};   // Button hover
ImVec4 _buttonActiveColor{0.4f, 0.4f, 0.4f, 1.0f};    // Button active
```

### Style Configuration
```cpp
void UIManager::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Spacing and sizing
    style.WindowRounding = 0.0f;      // Sharp corners
    style.ChildRounding = 0.0f;
    style.FrameRounding = 3.0f;       // Slight button rounding
    style.WindowBorderSize = 2.0f;    // Visible borders
    style.FrameBorderSize = 1.0f;
    
    // Colors applied from theme
    style.Colors[ImGuiCol_WindowBg] = _panelColor;
    style.Colors[ImGuiCol_Border] = _borderColor;
    style.Colors[ImGuiCol_Text] = _textColor;
    // ... additional color assignments
}
```

## Keyboard Shortcuts Integration

### Bi-directional Control System

| Key | Function | UI Control | Synchronization |
|-----|----------|------------|-----------------|
| **P** | Wireframe Mode | Checkbox | ✅ Bi-directional |
| **V** | Show Vertices | Checkbox | ✅ Bi-directional |
| **1** | Orthographic | Checkbox | ✅ Bi-directional |
| **X** | Auto Rotation | Checkbox | ✅ Bi-directional |
| **R** | Reset Camera | Button | ✅ Trigger action |
| **WASD** | Camera Movement | Position Display | ✅ Real-time update |
| **Mouse** | Look Around | Position Display | ✅ Real-time update |
| **Scroll** | Zoom | Speed Settings | ✅ Configurable |

### Implementation Details

```cpp
// Keyboard input triggers UI update
void InputManager::key_callback(...) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        _wireframeMode = !_wireframeMode;
        if (_onWireframeToggle) {
            _onWireframeToggle(_wireframeMode);  // Notifies App
        }
    }
}

// App coordinates between InputManager and UIManager
void App::handleWireframeToggle(bool wireframeMode) {
    _wireframeMode = wireframeMode;
    
    // Update UI to reflect keyboard input
    UIState currentState = _uiManager->getState();
    currentState.wireframeMode = wireframeMode;
    _uiManager->updateState(currentState);
}
```

## Build Integration

### Dependencies
```makefile
# ImGui source files
IMGUI_SRC := lib/imgui/imgui.cpp \
             lib/imgui/imgui_demo.cpp \
             lib/imgui/imgui_draw.cpp \
             lib/imgui/imgui_tables.cpp \
             lib/imgui/imgui_widgets.cpp \
             lib/imgui/backends/imgui_impl_glfw.cpp \
             lib/imgui/backends/imgui_impl_opengl3.cpp

# Include paths
INCLUDES = -Ilib/imgui -Ilib/imgui/backends

# Library linking
LIBS = -lGL -lglfw -ldl -lm -pthread
```

### Compilation
```bash
# Build with ImGui integration
make clean && make

# Run with UI
./scop resources/objects/model.obj
```

## Technical Features

### Performance Optimizations

1. **State Caching**: UI state only updates when changed
2. **Layout Stability**: Render area calculated once per frame
3. **Selective Updates**: Only visible UI elements update data
4. **Buffer Management**: FPS history uses circular buffer

### Error Handling

1. **Graceful Degradation**: UI continues functioning if 3D rendering fails
2. **State Validation**: UI state bounds checking for sliders
3. **Resource Management**: Proper ImGui context lifecycle
4. **OpenGL State**: Restoration after UI rendering

### Cross-Platform Compatibility

1. **GLFW Backend**: Works on Linux, Windows, macOS
2. **OpenGL 3.3+**: Modern graphics pipeline support
3. **ImGui Portability**: Native look and feel
4. **Dynamic Sizing**: Adapts to different screen resolutions

## Usage Guide

### Getting Started

1. **Launch Application**
   ```bash
   ./scop path/to/model.obj
   ```

2. **Navigate Interface**
   - Left panel: Controls and information
   - Right area: 3D visualization
   - Keyboard shortcuts work globally

3. **Interact with Controls**
   - Click checkboxes to toggle modes
   - Drag sliders to adjust settings
   - Use keyboard shortcuts for quick access

### Best Practices

1. **Performance Monitoring**: Use FPS graph to assess impact of settings
2. **Camera Management**: Reset camera if view becomes disoriented
3. **Speed Adjustment**: Tune camera/rotation speeds for comfort
4. **File Information**: Check mesh statistics before adjusting settings

### Troubleshooting

1. **No 3D Model Visible**
   - Check if wireframe mode reveals geometry
   - Reset camera to default position
   - Verify model file path and format

2. **UI Not Responding**
   - Ensure keyboard focus is on application window
   - Check that ImGui context is properly initialized

3. **Performance Issues**
   - Monitor FPS in performance section
   - Reduce vertex visualization for complex models
   - Consider wireframe mode for analysis

## Future Enhancement Opportunities

### Planned Features

1. **File Browser**: Built-in OBJ file selection dialog
2. **Material Editor**: Real-time material property adjustment
3. **Lighting Controls**: Interactive light positioning and color
4. **Export System**: Screenshot and model export functionality
5. **Animation Timeline**: Keyframe-based animation system
6. **Shader Editor**: Real-time shader modification interface

### Advanced UI Components

1. **3D Manipulation Widgets**: Gizmos for direct model interaction
2. **Property Grids**: Hierarchical parameter organization
3. **Docking System**: Customizable panel arrangement
4. **Theme System**: Multiple color schemes and layouts
5. **Tooltip System**: Contextual help and information

### Integration Enhancements

1. **Plugin Architecture**: Extensible rendering and import systems
2. **Scripting Support**: Lua/Python integration for automation
3. **Network Features**: Collaborative editing and sharing
4. **VR Support**: Virtual reality viewing modes

This UI implementation provides a professional, user-friendly interface that enhances the 3D rendering experience while maintaining the technical depth required for the SCOP project. The system is designed to be maintainable, extensible, and performant across different platforms and use cases.
