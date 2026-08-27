# Maya Real-time Lighting Plugin (X-ray 3 Prototype)

A real-time global illumination plugin for Autodesk Maya, providing interactive lighting solutions similar to Unreal's Lumen.

## Features

- **Real-time Global Illumination** - Dynamic lighting with bouncing light
- **Interactive Viewport Rendering** - See lighting changes in real-time
- **GPU-Accelerated** - Uses CUDA for fast computation
- **Material Support** - Basic material system with diffuse, metallic, roughness
- **Dynamic Shadows** - Real-time shadow casting
- **Performance Optimized** - Efficient rendering pipeline

## Project Structure

```
MayaRealtimeLighting/
├── src/
│   ├── plugin/              # Main Maya plugin code
│   │   ├── plugin.cpp
│   │   ├── plugin.h
│   │   └── pluginMain.cpp
│   ├── viewport/            # Viewport rendering system
│   │   ├── viewportRenderer.cpp
│   │   ├── viewportRenderer.h
│   │   ├── shader.cpp
│   │   └── shader.h
│   ├── core/                # Core lighting engine
│   │   ├── lightingEngine.cpp
│   │   ├── lightingEngine.h
│   │   ├── raytracer.cu     # CUDA ray tracing
│   │   ├── scene.cpp
│   │   └── scene.h
│   ├── geometry/            # Mesh/geometry handling
│   │   ├── mesh.cpp
│   │   ├── mesh.h
│   │   ├── bvh.cpp          # Bounding Volume Hierarchy
│   │   └── bvh.h
│   ├── material/            # Material system
│   │   ├── material.cpp
│   │   └── material.h
│   └── utils/               # Utility functions
│       ├── math.h
│       ├── logger.h
│       └── timer.h
├── shaders/
│   ├── raytracing.cu        # CUDA kernels
│   ├── display.glsl         # Display shader
│   └── lighting.glsl        # Lighting calculations
├── cmake/
│   ├── CMakeLists.txt       # Main CMake config
│   ├── FindMaya.cmake       # Maya detection
│   └── FindCUDA.cmake       # CUDA detection
├── docs/
│   ├── ARCHITECTURE.md      # System architecture
│   ├── BUILD.md             # Build instructions
│   ├── API.md               # Plugin API documentation
│   └── CUDA_SETUP.md        # CUDA setup guide
├── examples/
│   ├── simple_scene.ma      # Example Maya scene
│   └── test_scene.py        # Test scene script
├── CMakeLists.txt
├── .gitignore
└── LICENSE
```

## Quick Start

### Prerequisites

- Autodesk Maya 2020+ (Windows, Linux, or macOS)
- Visual Studio 2019+ (Windows) or GCC/Clang (Linux/Mac)
- NVIDIA CUDA Toolkit 11.0+
- NVIDIA GPU with Compute Capability 7.0+ (GeForce RTX, Tesla)
- CMake 3.16+

### Building

```bash
# Clone the repository
git clone https://github.com/mondsoura2311/MayaRealtimeLighting.git
cd MayaRealtimeLighting

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DMAYA_LOCATION="C:/Program Files/Autodesk/Maya2025" ..

# Build
cmake --build . --config Release

# Plugin will be in: build/Release/plug-ins/
```

### Installation

1. Copy the compiled `.mll` (Windows), `.so` (Linux), or `.bundle` (macOS) to Maya's plugin folder:
   - Windows: `C:\Program Files\Autodesk\Maya2025\bin\plug-ins\`
   - Linux: `~/maya/2025/plug-ins/`
   - macOS: `~/Library/Preferences/Autodesk/maya/2025/plug-ins/`

2. Open Maya and go to `Windows > Settings/Preferences > Plug-in Manager`

3. Search for "MayaRealtimeLighting" and click Load

4. In the viewport, select a mesh and enable real-time lighting from the plugin menu

## Architecture Overview

### Rendering Pipeline

```
Maya Scene Graph
       ↓
  Geometry Extraction (mesh, lights, camera)
       ↓
  BVH Construction (acceleration structure)
       ↓
  GPU Transfer (upload to VRAM)
       ↓
  CUDA Kernel Execution (ray tracing)
       ↓
  Accumulation Buffer
       ↓
  OpenGL Display to Viewport
       ↓
  Maya Viewport Output
```

### Core Components

1. **Plugin System** - Maya plugin initialization and lifecycle
2. **Viewport Renderer** - Custom OpenGL viewport integration
3. **Lighting Engine** - Global illumination calculations
4. **Ray Tracer** - CUDA-based GPU ray tracing
5. **Scene Manager** - Geometry and light management
6. **Material System** - PBR material support
7. **BVH Acceleration** - Fast ray-geometry intersection

## Performance

- Interactive frame rates (30-60 FPS) on RTX 2060+
- Progressive refinement for quality
- Real-time updates with scene changes
- GPU memory efficient

## Limitations (Prototype)

- Limited material types (diffuse, metallic, roughness)
- Single bounce GI (extensible to multi-bounce)
- No hair/particle support yet
- Basic shadow mapping
- Single GPU support

## Roadmap

- [ ] Multi-bounce global illumination
- [ ] Advanced material system (subsurface scattering, anisotropic)
- [ ] Denoising (OptiX AI Denoiser)
- [ ] Volumetric lighting
- [ ] Hair/strand rendering
- [ ] Multi-GPU support
- [ ] Arnold material interop
- [ ] OSL shader support

## Development

For detailed development information, see:
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Build Instructions](docs/BUILD.md)
- [API Reference](docs/API.md)
- [CUDA Setup Guide](docs/CUDA_SETUP.md)

## Contributing

Contributions welcome! Please ensure:
- Code follows C++17 standards
- CUDA code is optimized for performance
- All changes tested on supported Maya versions
- Documentation updated accordingly

## License

MIT License - See LICENSE file

## Disclaimer

This is a prototype/learning project. Not intended for production use. Use at your own risk.

## References

- [Autodesk Maya API Documentation](https://help.autodesk.com/view/MAYAAPI/2025/)
- [NVIDIA CUDA Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- [Real-time Rendering by Akenine-Möller et al.](https://www.realtimerendering.com/)
- [Path Tracing in Production](https://www.nvidia.com/en-us/research/ai-computing/)
