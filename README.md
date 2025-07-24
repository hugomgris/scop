# scop-fdf3d

🚀 Hybrid project for 42's `scop` and a C++ `FDF3D` side project.  
🎮 Goal: Build a modular 3D object viewer with OpenGL in C++, supporting multiple rendering modes (solid, wireframe, FDF-style, textured).

---

## 🎯 Features
- .obj file parsing
- OpenGL rendering with shaders
- Lighting and texturing
- Custom rendering modes (FDF heightmap style!)
- Camera movement and zoom
- Ready to be expanded into a game engine

---

## 🗺️ Roadmap

| Milestone | Description |
|----------|-------------|
| ✅ 0. Setup | GLFW + GLAD, window opens |
| 🔜 1. Mesh loading | Parse `.obj`, render mesh |
| 🔜 2. Lighting | Basic normals + lighting |
| 🔜 3. Modes | Wireframe, FDF-style toggle |
| 🔜 4. Texture | Load image, apply to UVs |

---

## 📝 Dev Logs

Logs stored in `devlogs/` per milestone, useful for documentation and 42 defense.

---

## 🛠️ Build Instructions

```bash
mkdir build && cd build
cmake ..
make
./scop-fdf3d

