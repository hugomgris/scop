# CRT Post-Processing Implementation

## Overview

This document describes the implementation of a CRT (Cathode Ray Tube) post-processing effect system for the SCOP project. The system applies authentic CRT visual effects to the 3D rendered content while preserving the UI overlay functionality.

## Features

- **Viewport-Only Application**: CRT effects are applied only to the 3D rendered content, not the UI panels
- **Real-time Toggle**: Can be enabled/disabled via UI checkbox or keyboard shortcut ('C' key)
- **Authentic CRT Effects**:
  - Barrel distortion for curved screen simulation
  - Scanlines with proper intensity modulation
  - Vignette effect for screen edge darkening
  - Chromatic aberration for color separation
  - Screen curvature and aspect ratio preservation

## Architecture

### Core Components

1. **PostProcessor Class** (`include/PostProcessor.hpp`, `src/renderer/PostProcessor.cpp`)
2. **CRT Shader** (`resources/shaders/CRT_PostProcess.shader`)
3. **App Integration** (Modified `src/app/App.cpp`)
4. **UI Integration** (Modified `src/ui/UIManager.cpp`)
5. **Input Handling** (Modified `src/app/InputManager.cpp`)

## Implementation Details

### 1. PostProcessor Class

#### Purpose
Manages framebuffer objects for off-screen rendering and applies post-processing effects.

#### Key Features
- **Framebuffer Management**: Creates and manages off-screen rendering targets
- **Quad Rendering**: Renders fullscreen quads for post-processing
- **Dynamic Resizing**: Handles viewport size changes
- **Time-based Effects**: Supports animated effects with time uniforms

#### Core Methods
```cpp
PostProcessor(int width, int height);           // Constructor with initial size
void bind();                                    // Bind framebuffer for off-screen rendering
void unbind();                                  // Return to default framebuffer
void render();                                  // Render post-processed result
void resize(int width, int height);            // Handle viewport size changes
void setEnableCRT(bool enable);                // Toggle CRT effect
void updateTime(float time);                   // Update time for animations
```

#### Framebuffer Setup
- **Color Attachment**: RGB texture for rendered scene
- **Depth Attachment**: Renderbuffer for depth testing
- **Viewport Matching**: Framebuffer size matches viewport dimensions

### 2. CRT Shader Implementation

#### Shader Location
`resources/shaders/CRT_PostProcess.shader`

#### Uniforms
```glsl
uniform sampler2D u_screenTexture;    // Input rendered scene
uniform float u_time;                 // Time for animations
uniform vec2 u_resolution;            // Screen resolution
uniform bool u_enableCRT;             // Toggle CRT effect
```

#### Effect Functions

**Barrel Distortion (`curveRemapUV`)**
```glsl
vec2 curveRemapUV(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(6.0, 4.0);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}
```

**Scanline Effect (`scanlineEffect`)**
```glsl
float scanlineEffect(vec2 uv, float time) {
    float scanline = sin(uv.y * 800.0) * 0.04;
    float slowscan = sin(uv.y * 80.0 + time * 2.0) * 0.02;
    return 1.0 - (scanline + slowscan);
}
```

**Vignette Effect (`vignetteEffect`)**
```glsl
float vignetteEffect(vec2 uv) {
    uv *= 1.0 - uv.yx;
    float vig = uv.x * uv.y * 15.0;
    return pow(vig, 0.25);
}
```

**Chromatic Aberration (`chromaticAberration`)**
```glsl
vec3 chromaticAberration(sampler2D tex, vec2 uv) {
    vec2 offset = vec2(0.002, 0.0);
    float r = texture(tex, uv - offset).r;
    float g = texture(tex, uv).g;
    float b = texture(tex, uv + offset).b;
    return vec3(r, g, b);
}
```

### 3. Render Pipeline Integration

#### Modified Render Loop (App.cpp)

**Phase 1: Off-screen 3D Rendering**
```cpp
// Update PostProcessor size if needed
if (viewportWidth > 0 && viewportHeight > 0) {
    static int lastWidth = 0, lastHeight = 0;
    if (lastWidth != viewportWidth || lastHeight != viewportHeight) {
        _postProcessor->resize(viewportWidth, viewportHeight);
        lastWidth = viewportWidth;
        lastHeight = viewportHeight;
    }
}

// Bind PostProcessor framebuffer for off-screen 3D scene rendering
_postProcessor->bind();

// Ensure proper OpenGL state for 3D rendering
glDisable(GL_SCISSOR_TEST);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CCW);

// Clear and render 3D scene
setClearColor(Colors::BLACK_CHARCOAL_1);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Render 3D content (mesh, materials, etc.)
```

**Phase 2: Post-Processing Application**
```cpp
// Unbind framebuffer and return to default framebuffer
_postProcessor->unbind();

// Set up full screen for rendering the post-processed result
glViewport(0, 0, 1920, 1080);

// Clear the entire screen first
setClearColor(Colors::BLACK_CHARCOAL_1);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Set CRT mode on the post processor
_postProcessor->setEnableCRT(_uiManager->getState().enableCRT);

// Set up scissor test to only render to viewport area
glEnable(GL_SCISSOR_TEST);
glScissor(viewportX, 1080 - viewportY - viewportHeight, viewportWidth, viewportHeight);

// Disable depth testing and face culling for post-processing quad
glDisable(GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);

// Render the post-processed 3D scene to the viewport area
_postProcessor->render();

// Disable scissor test
glDisable(GL_SCISSOR_TEST);
```

**Phase 3: UI Overlay**
```cpp
// Now render UI on top of everything
_uiManager->render();
```

### 4. UI Integration

#### UIState Extension
```cpp
struct UIState {
    // ... existing fields ...
    bool enableCRT = false;  // Added CRT toggle state
};
```

#### UI Controls (UIManager.cpp)
```cpp
// CRT Effect checkbox in rendering controls
bool crtEffect = _state.enableCRT;
if (ImGui::Checkbox("CRT Effect [C]", &crtEffect)) {
    _state.enableCRT = crtEffect;
    if (onCRTModeChanged) {
        onCRTModeChanged(crtEffect);
    }
}
```

#### Callback System
```cpp
// In App.cpp setupUICallbacks()
_uiManager->onCRTModeChanged = [this](bool enableCRT) {
    this->handleCRTToggle(enableCRT);
    // Keep InputManager in sync
    if (_inputManager) {
        _inputManager->setEnableCRT(enableCRT);
    }
};
```

### 5. Input Handling

#### Keyboard Toggle (InputManager.cpp)
```cpp
// In key_callback function
case GLFW_KEY_C:
    if (action == GLFW_PRESS) {
        _enableCRT = !_enableCRT;
        if (_crtToggleCallback) {
            _crtToggleCallback(_enableCRT);
        }
        std::cout << "CRT Effect " << (_enableCRT ? "ON" : "OFF") << std::endl;
    }
    break;
```

#### State Synchronization
The system maintains state consistency between:
- PostProcessor internal state
- UIManager UI state
- InputManager keyboard state
- App application state

## Technical Considerations

### OpenGL State Management

**Framebuffer Operations**
- Proper binding/unbinding sequence
- Viewport management for different render targets
- Scissor test for viewport-only rendering
- **Critical**: Single clearing per render cycle - avoid multiple `glClear()` calls
- **Multi-material Considerations**: Framebuffer clearing must happen only once per frame

**Depth Testing**
- Enabled for 3D scene rendering with `glDepthMask(GL_TRUE)`
- Disabled for post-processing quad
- Proper state restoration between render phases
- **Face Culling**: Disabled for complex models to avoid winding order issues

**Multi-Material Rendering Considerations**
- **Buffer Management**: Use static temporary buffers instead of per-frame allocation
- **State Preservation**: Avoid clearing framebuffer mid-render cycle
- **Texture Binding**: Ensure proper texture unit management across material groups
- **Performance**: `GL_STREAM_DRAW` for frequently updated temporary buffers

**Texture Management**
- Single color attachment for scene capture
- Proper texture filtering for post-processing
- Memory management for dynamic resizing

### Performance Optimizations

**Conditional Rendering**
```glsl
// Shader optimization for disabled CRT
if (!u_enableCRT) {
    gl_FragColor = texture(u_screenTexture, v_texCoord);
    return;
}
```

**Static Allocations**
- Pre-allocated framebuffer objects
- Reused VAO/VBO for fullscreen quad
- **Improved**: Static temporary buffers for material rendering instead of per-frame allocation

**Multi-Material Rendering Optimizations**
```cpp
// Optimized buffer reuse for material groups
static unsigned int tempIBO = 0;
if (tempIBO == 0) {
    glGenBuffers(1, &tempIBO);  // Create once
}
// Reuse with GL_STREAM_DRAW for each material group
glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
```

**Framebuffer Management**
- Single clear operation per render cycle
- Avoid redundant state changes
- Efficient viewport transitions
- Minimal dynamic allocations

**Scissor Testing**
- Limits post-processing to viewport area only
- Reduces fragment shader workload
- Preserves UI rendering performance

### Error Handling

**OpenGL Error Checking**
```cpp
GLCall(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));
GLCall(glViewport(0, 0, _width, _height));
```

**Framebuffer Completeness**
```cpp
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Error: Framebuffer is not complete!" << std::endl;
}
```

**Shader Compilation**
- Proper error reporting for shader compilation failures
- Uniform location validation
- Graceful fallback for missing uniforms

## Usage

### Basic Usage
1. **UI Toggle**: Click the "CRT Effect [C]" checkbox in the rendering controls panel
2. **Keyboard Toggle**: Press 'C' key to toggle CRT effect on/off
3. **Real-time Feedback**: Console output confirms state changes

### Integration Points
- **Shader Loading**: Ensure `CRT_PostProcess.shader` is in `resources/shaders/`
- **Makefile**: PostProcessor.cpp must be included in compilation
- **Dependencies**: Requires OpenGL 3.3+, GLM for math operations

## Debugging

### Common Issues

**Framebuffer Problems**
- Check framebuffer completeness status
- Verify texture format compatibility
- Ensure proper viewport sizing

**Critical: Double Framebuffer Clearing Issue** ⚠️
- **Problem**: Missing faces in complex multi-material models (e.g., Ninja.obj, Otacon.obj)
- **Root Cause**: `renderWithMaterials()` was clearing the framebuffer after main render loop had already cleared it
- **Symptoms**: Faces missing from certain angles, "void" areas in complex models, inconsistent rendering
- **Solution**: Remove redundant `glClear()` calls in material rendering functions
- **Impact**: Critical for multi-material model rendering integrity
- **Code Fix**: Comment out clearing in `renderWithMaterials()` since main loop handles it
```cpp
// WRONG - Double clearing causes corruption:
void renderWithMaterials() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ❌ Removes this
    // ... material rendering
}

// CORRECT - Let main render loop handle clearing:
void renderWithMaterials() {
    // Don't clear here - already done in main render loop ✅
    // ... material rendering
}
```

**Buffer Management Performance Issues**
- **Problem**: Frame drops and potential OpenGL state corruption with complex models
- **Cause**: Creating/deleting index buffers every frame for each material group
- **Solution**: Use static temporary buffer with `GL_STREAM_DRAW` for reuse
- **Optimization**: Reduces buffer allocation overhead significantly

**Rendering Order Issues**
- Verify framebuffer bind/unbind sequence
- Check scissor test setup
- Confirm UI rendering happens last

**Viewport and Face Rendering Issues**
- **Problem**: Missing faces or incorrect model positioning
- **Cause**: Viewport conflicts between framebuffer and final rendering
- **Solution**: Remove viewport calls before binding PostProcessor framebuffer
- **Key Fix**: Ensure `PostProcessor::bind()` sets viewport to framebuffer dimensions
- **Face Culling**: Enable proper face culling for 3D rendering, disable for post-processing quad

**OpenGL State Management**
- **3D Rendering State**: Enable depth testing, face culling, disable scissor test
- **Post-Processing State**: Disable depth testing and face culling for fullscreen quad
- **UI Rendering State**: Restore appropriate state for ImGui rendering

**Shader Compilation**
- Validate shader file path
- Check uniform names match exactly
- Verify OpenGL version compatibility

### Debug Output
The system provides console output for:
- PostProcessor initialization status
- CRT effect toggle confirmations
- Rendering mode changes
- OpenGL error reporting

### Lessons Learned & Best Practices

**Critical Issues Resolved During Development:**

1. **Framebuffer Clearing Conflicts**
   - **Issue**: Multiple `glClear()` calls per frame corrupted multi-material rendering
   - **Lesson**: Framebuffer should be cleared exactly once per render cycle
   - **Best Practice**: Centralize clearing in main render loop, not in sub-functions

2. **Buffer Management for Complex Models**
   - **Issue**: Per-frame buffer allocation caused performance drops and state corruption
   - **Lesson**: Static buffer reuse is essential for multi-material models
   - **Best Practice**: Use `GL_STREAM_DRAW` for frequently updated temporary data

3. **Face Culling Complexity**
   - **Issue**: Mixed face winding in complex models caused missing faces
   - **Lesson**: Disable face culling for complex imported models when debugging
   - **Best Practice**: Test with culling disabled first, then optimize per-model

4. **Viewport State Management**
   - **Issue**: Incorrect viewport setup caused model positioning problems
   - **Lesson**: Let framebuffer binding handle its own viewport, don't override
   - **Best Practice**: Calculate matrices with actual viewport dimensions

**Development Workflow Recommendations:**
- Test with simple models first (42.obj), then complex multi-material models
- Always verify console output for material loading confirmation
- Monitor for "double clearing" patterns in render functions
- Use wireframe mode to debug face rendering issues

## Future Enhancements

### Potential Improvements
1. **Additional CRT Effects**:
   - Screen burn-in simulation
   - Phosphor persistence
   - Color temperature adjustment
   - Screen flicker effects

2. **Performance Optimizations**:
   - Multiple render target support
   - Compute shader implementation
   - Temporal effects caching

3. **User Customization**:
   - Adjustable effect intensity
   - Multiple CRT presets
   - Real-time parameter tweaking

### Extensibility
The post-processing system is designed for easy extension:
- Additional shader effects can be added to the CRT shader
- Multiple post-processing passes can be chained
- Effect parameters can be exposed via UI controls

## Conclusion

The CRT post-processing implementation provides an authentic retro visual effect while maintaining clean separation between 3D content and UI elements. Through iterative development and debugging, the system has evolved to be robust, performant, and reliable across different model complexities.

**Key Achievements:**
- ✅ **Multi-Material Compatibility**: Handles complex models (Ninja.obj with 36 materials, Otacon.obj with 31 materials)
- ✅ **Performance Optimized**: Efficient buffer management and single-pass framebuffer operations
- ✅ **State Management**: Robust OpenGL state handling with proper restoration
- ✅ **User Experience**: Seamless toggle functionality with real-time feedback
- ✅ **Viewport Integration**: CRT effects apply only to 3D content, preserving UI clarity

**Technical Robustness:**
The system successfully handles edge cases and complex scenarios that emerged during development, including framebuffer state conflicts, multi-material rendering challenges, and viewport management complexities. The implementation serves as a solid foundation for future post-processing effects.

**Maintainability:**
The codebase is well-documented with clear separation of concerns, comprehensive error handling, and extensive debugging output. The modular design facilitates easy extension and modification without affecting core functionality.
