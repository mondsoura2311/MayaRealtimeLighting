# Maya Real-time Lighting Plugin - Architecture Documentation

## System Overview

The plugin follows a modular architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────┐
│           Maya Application                           │
│  (Scene Graph, Viewport, Node Architecture)          │
└──────────────────┬──────────────────────────────────┘
                   │
         ┌─────────▼──────────┐
         │   Plugin Interface │ (pluginMain.cpp)
         │  - Load/Unload     │
         │  - Command/Node    │
         │  - Viewport Event  │
         └─────────┬──────────┘
                   │
      ┌────────────┼────────────┐
      │            │            │
   ┌──▼──┐    ┌───▼────┐   ┌──▼───┐
   │Scene│    │Viewport│   │Render│
   │Mgr  │    │Manager │   │Queue │
   └──┬──┘    └───┬────┘   └──┬───┘
      │           │            │
      └─────┬─────┴────┬───────┘
            │          │
       ┌────▼──────────▼────┐
       │  Lighting Engine   │
       │  - GI Calculation  │
       │  - Material Eval   │
       │  - Accumulation    │
       └────┬───────────┬───┘
            │           │
       ┌────▼────┐  ┌───▼─────┐
       │GPU Core │  │ BVH Tree│
       │(CUDA RT)│  │(Accel)  │
       └─────────┘  └─────────┘
```

## Core Components

### 1. Plugin System (`src/plugin/`)

**Files:**
- `pluginMain.cpp` - Entry point
- `plugin.cpp/h` - Plugin class definition

**Responsibilities:**
- Initialize/shutdown plugin
- Register commands and nodes
- Hook into Maya viewport
- Handle scene events

**Key Classes:**
```cpp
class MayaRealtimeLightingPlugin : public MPxPlugin
{
    // Lifecycle management
    // Command/Node registration
    // Viewport integration
};
```

### 2. Viewport Renderer (`src/viewport/`)

**Files:**
- `viewportRenderer.cpp/h` - Main renderer
- `shader.cpp/h` - OpenGL shader management

**Responsibilities:**
- Custom viewport override
- OpenGL context management
- Render queue processing
- Output to Maya viewport

**Data Flow:**
```
Maya Viewport Event
        ↓
  Renderer::render()
        ↓
  Prepare Scene Data
        ↓
  Execute Lighting Engine
        ↓
  Display Result
        ↓
  Maya Viewport Output
```

### 3. Lighting Engine (`src/core/`)

**Files:**
- `lightingEngine.cpp/h` - Main engine
- `scene.cpp/h` - Scene management
- `raytracer.cu` - GPU ray tracing

**Responsibilities:**
- Coordinate lighting calculations
- Manage GPU memory
- Accumulate samples
- Handle tone mapping

**Processing Pipeline:**
```
1. Extract Scene
   - Geometry (meshes, normals, UVs)
   - Lights (position, intensity, color)
   - Camera (resolution, FOV, transforms)
   - Materials (diffuse, metallic, roughness)

2. Build Acceleration Structure
   - BVH construction on CPU
   - Transfer to GPU

3. Ray Tracing Kernel
   - Primary rays from camera
   - Material evaluation
   - Light sampling
   - Shadow rays
   - Indirect bounces (GI)

4. Accumulation
   - Temporal filtering
   - Variance reduction
   - Denoising (future)

5. Tone Mapping & Display
   - Color correction
   - Gamma adjustment
   - UI overlay
```

### 4. Geometry Management (`src/geometry/`)

**Files:**
- `mesh.cpp/h` - Mesh representation
- `bvh.cpp/h` - Bounding Volume Hierarchy

**BVH Structure:**
```cpp
struct BVHNode {
    BBox bbox;           // Bounding box
    int leftChild;        // Index of left child (-1 = leaf)
    int rightChild;       // Index of right child
    int triangleStart;    // Start index for leaf nodes
    int triangleCount;    // Number of triangles (leaf only)
};
```

**Ray-Triangle Intersection:**
- Möller-Trumbore algorithm
- Optimized for CUDA
- Returns: hit distance, barycentric coordinates, triangle ID

### 5. Material System (`src/material/`)

**Files:**
- `material.cpp/h` - Material definition

**Material Properties:**
```cpp
struct Material {
    float3 albedo;       // Base color
    float3 emission;     // Emissive color
    float metallic;      // 0 = dielectric, 1 = metal
    float roughness;     // Surface roughness
    float ior;           // Index of refraction
    uint flags;          // Material flags
};
```

**BRDF Evaluation:**
- Diffuse: Lambertian
- Specular: GGX microfacet
- Metal: Fresnel-dependent reflection

### 6. Utilities (`src/utils/`)

**Files:**
- `math.h` - Vector/matrix operations
- `logger.h` - Debug logging
- `timer.h` - Performance profiling

## Memory Architecture

### CPU Memory
```
Scene Data (MB)
├── Vertex Buffer
├── Index Buffer
├── Material Buffer
├── Light Buffer
├── Transform Buffer
└── BVH Nodes
```

### GPU Memory (VRAM)
```
GPU Memory (GB)
├── BVH Tree
├── Triangle Data
├── Material Properties
├── Light Information
├── Accumulation Buffer
├── Work Queues
└── Temporary Buffers
```

## Ray Tracing Pipeline (CUDA)

### Kernel Organization
```cuda
// Global illumination kernel
__global__ void rayTracingKernel(
    uint2 launchIndex,      // Pixel coordinates
    uint sampleIndex,       // Current sample
    const Scene scene,      // Scene data
    const BVHNode* bvh,     // Acceleration structure
    float4* outputBuffer    // Output pixels
)
{
    // 1. Generate primary ray
    Ray primaryRay = generateCameraRay(launchIndex);
    
    // 2. Trace ray through BVH
    Intersection hit = traceRay(primaryRay, bvh);
    
    // 3. Evaluate material
    float3 radiance = evaluateMaterial(hit);
    
    // 4. Sample lights
    radiance += sampleDirectLighting(hit, scene);
    
    // 5. Generate bounce ray (GI)
    Ray bounceRay = sampleIndirectRay(hit);
    Intersection indirectHit = traceRay(bounceRay, bvh);
    radiance += evaluateMaterial(indirectHit) * 0.5f;
    
    // 6. Accumulate
    accumulate(outputBuffer, launchIndex, radiance);
}
```

## Threading Model

### CPU Thread
```
Main Thread (Maya)
├── Viewport Event → Trigger Render
├── Scene Update Detection
├── BVH Rebuild (if needed)
└── GPU Launch
```

### GPU Thread Hierarchy
```
Grid (2D)  ← One block per tile
├── Block (16×16 threads)
│   ├── 256 threads
│   ├── Shared memory for acceleration
│   └── Cooperative processing
└── Each Thread
    ├── Process one pixel
    ├── Trace rays
    └── Update accumulation buffer
```

## Performance Considerations

### Optimization Strategies

1. **Spatial Coherence**
   - BVH prioritizes spatial locality
   - Reduces memory access patterns

2. **Batch Processing**
   - Tile-based rendering (16×16 pixels)
   - Improves cache efficiency

3. **Divergence Minimization**
   - Coherent ray paths
   - Warp-friendly branching

4. **Memory Coalescing**
   - Linear memory access patterns
   - Optimized data layout

5. **Register Usage**
   - Target <128 registers/thread
   - Maximize occupancy

## Extension Points

### Adding New Features

1. **New Material Type**
   - Extend `Material` struct
   - Add BRDF evaluation in kernel
   - Update material UI

2. **Post-processing**
   - Add compute kernel
   - Insert before display shader

3. **Denoising**
   - Integrate OptiX AI Denoiser
   - Run on accumulation buffer

4. **Advanced Lighting**
   - Area lights
   - Importance sampling
   - Caustics

## Error Handling

- CUDA error checking at every API call
- Graceful fallback to CPU rendering
- Comprehensive logging system
- User notification through Maya viewport

## Future Architecture Evolution

1. Multi-GPU support (NVIDIA NVLink)
2. Streaming for large scenes
3. Machine learning denoising
4. Real-time light probes
5. Dynamic scene updates without BVH rebuild
