# Maya Real-time Lighting Plugin - API Reference

## Core Classes

### MayaRealtimeLightingPlugin

Main plugin class that integrates with Maya.

```cpp
class MayaRealtimeLightingPlugin
{
public:
    // Lifecycle
    MStatus initialize(MFnPlugin& fnPlugin);
    MStatus shutdown();
    
    // Control
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // Accessors
    ViewportRenderer* getViewportRenderer() const;
    LightingEngine* getLightingEngine() const;
};
```

**Methods:**
- `initialize(fnPlugin)` - Initialize plugin and register with Maya
- `shutdown()` - Clean up and unregister from Maya
- `setEnabled(enabled)` - Enable/disable real-time lighting
- `isEnabled()` - Check if real-time lighting is active

---

## Viewport System

### ViewportRenderer

Manages OpenGL rendering to Maya viewport.

```cpp
class ViewportRenderer
{
public:
    // Lifecycle
    bool initialize();
    void shutdown();
    
    // Control
    void enableRendering();
    void disableRendering();
    bool isRenderingEnabled() const;
    
    // Render
    void render();
    void setViewportSize(int width, int height);
    
    // Camera
    void setCameraMatrix(const mat4& view, const mat4& projection);
    const mat4& getViewMatrix() const;
    const mat4& getProjectionMatrix() const;
    
    // Output
    uint32_t getOutputTextureID() const;
};
```

**Methods:**
- `initialize()` - Create OpenGL framebuffer and textures
- `shutdown()` - Clean up OpenGL resources
- `enableRendering()` / `disableRendering()` - Toggle rendering
- `render()` - Render current frame
- `setViewportSize(width, height)` - Update output size
- `setCameraMatrix(view, projection)` - Set camera matrices
- `getOutputTextureID()` - Get OpenGL texture handle

**Example Usage:**
```cpp
ViewportRenderer renderer;
renderer.initialize();
renderer.setViewportSize(1280, 720);
renderer.enableRendering();

// Each frame:
renderer.render();

renderer.shutdown();
```

---

## Lighting Engine

### LightingEngine

Core real-time lighting calculation engine using CUDA.

```cpp
class LightingEngine
{
public:
    // Lifecycle
    bool initialize();
    void shutdown();
    
    // Scene
    Scene* getScene() const;
    void updateScene();
    
    // Rendering
    void traceFrame(float4* outputBuffer, uint32_t width, uint32_t height, 
                   uint32_t sampleCount);
    
    // Settings
    void setMaxBounces(int bounces);
    int getMaxBounces() const;
    
    void setSamplesPerPixel(int samples);
    int getSamplesPerPixel() const;
    
    void setAccumulationEnabled(bool enabled);
    bool isAccumulationEnabled() const;
    
    void resetAccumulation();
};
```

**Methods:**
- `initialize()` - Initialize CUDA context and allocate buffers
- `shutdown()` - Clean up CUDA resources
- `getScene()` - Get scene object for adding geometry/lights
- `updateScene()` - Update BVH if scene changed
- `traceFrame()` - Launch CUDA ray tracing kernel
- `setMaxBounces(bounces)` - Set ray bounce depth (default: 2)
- `setSamplesPerPixel(samples)` - Set samples per pixel (default: 1)
- `setAccumulationEnabled(enabled)` - Enable temporal accumulation
- `resetAccumulation()` - Clear accumulation buffer

**Example Usage:**
```cpp
LightingEngine engine;
engine.initialize();
engine.setMaxBounces(3);
engine.setSamplesPerPixel(4);

Scene* scene = engine.getScene();
// Add geometry, lights, materials to scene

// Each frame:
engine.updateScene();
float4* outputBuffer = ...;
engine.traceFrame(outputBuffer, 1280, 720, 1);

engine.shutdown();
```

---

## Scene Management

### Scene

Manages scene geometry, lights, materials, and camera.

```cpp
class Scene
{
public:
    // Lifecycle
    bool initialize();
    void shutdown();
    void update();
    
    // Geometry
    void addMesh(std::shared_ptr<Mesh> mesh);
    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
    
    // Lighting
    void addLight(const Light& light);
    const std::vector<Light>& getLights() const;
    
    // Materials
    void addMaterial(std::shared_ptr<Material> material);
    const std::vector<std::shared_ptr<Material>>& getMaterials() const;
    
    // Camera
    void setCameraPosition(const float3& pos);
    void setCameraDirection(const float3& dir);
    void setCameraFOV(float fov);
    
    float3 getCameraPosition() const;
    float3 getCameraDirection() const;
    float getCameraFOV() const;
};
```

**Methods:**
- `addMesh(mesh)` - Add mesh to scene
- `addLight(light)` - Add light to scene
- `addMaterial(material)` - Add material to material library
- `setCameraPosition/Direction/FOV()` - Set camera
- `update()` - Rebuild acceleration structures if scene changed

**Example Usage:**
```cpp
Scene* scene = engine.getScene();

// Add a mesh
auto mesh = std::make_shared<Mesh>();
mesh->setVertices(vertices);
mesh->setIndices(indices);
mesh->build();
scene->addMesh(mesh);

// Add a light
Light light;
light.position = {5.0f, 8.0f, 5.0f};
light.color = {1.0f, 1.0f, 1.0f};
light.intensity = 1.5f;
scene->addLight(light);

// Set camera
scene->setCameraPosition({0, 3, 15});
scene->setCameraDirection({0, -0.2f, -1});
scene->setCameraFOV(45.0f);
```

---

## Geometry System

### Mesh

Represents a geometric mesh.

```cpp
class Mesh
{
public:
    // Data
    void setVertices(const std::vector<float3>& vertices);
    void setNormals(const std::vector<float3>& normals);
    void setTexCoords(const std::vector<float2>& texCoords);
    void setIndices(const std::vector<uint32_t>& indices);
    void setMaterialID(int materialID);
    
    // Build
    void build();
    
    // Getters
    const std::vector<Triangle>& getTriangles() const;
    const BBox& getBoundingBox() const;
    
    // Transform
    void setTransform(const mat4& transform);
    const mat4& getTransform() const;
};
```

**Methods:**
- `setVertices/Normals/TexCoords/Indices()` - Set geometry data
- `setMaterialID()` - Assign material to mesh
- `build()` - Convert data to triangle format and compute BVH
- `setTransform()` - Set mesh transformation matrix

**Example Usage:**
```cpp
auto mesh = std::make_shared<Mesh>();

// Define a simple cube
std::vector<float3> vertices = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
};

std::vector<uint32_t> indices = {
    0, 1, 2, 0, 2, 3,  // Front
    // ... more faces
};

mesh->setVertices(vertices);
mesh->setIndices(indices);
mesh->setMaterialID(0);
mesh->build();

scene->addMesh(mesh);
```

### BVH (Bounding Volume Hierarchy)

Acceleration structure for ray-mesh intersection.

```cpp
class BVH
{
public:
    void build(const std::vector<Triangle>& triangles);
    bool raycast(const Ray& ray, Intersection& intersection) const;
    
    const std::vector<BVHNode>& getNodes() const;
    const std::vector<Triangle>& getTriangles() const;
    bool isBuilt() const;
};
```

**Methods:**
- `build(triangles)` - Construct BVH from triangles (SAH based)
- `raycast(ray, intersection)` - Find closest intersection

---

## Material System

### Material

PBR material definition.

```cpp
struct Material {
    float3 albedo;      // Base color (0-1)
    float3 emission;    // Emissive color (0-1)
    float metallic;     // 0 = dielectric, 1 = metal
    float roughness;    // 0 = smooth, 1 = rough
    float ior;          // Index of refraction (default 1.5)
    uint32_t flags;     // Material flags
};
```

**Properties:**
- `albedo` - Diffuse/base color
- `emission` - Self-illuminated color
- `metallic` - Metallic property (0-1)
- `roughness` - Surface roughness (0-1)
- `ior` - Index of refraction for dielectrics

**Example Usage:**
```cpp
Material mat;
mat.albedo = {0.8f, 0.2f, 0.2f};      // Red
mat.metallic = 0.5f;                  // Half metallic
mat.roughness = 0.3f;                 // Smooth

auto material = std::make_shared<Material>(mat);
scene->addMaterial(material);
```

### MaterialSystem

Utility class for material evaluation.

```cpp
class MaterialSystem
{
public:
    static float3 evaluateBRDF(const Material& mat, const float3& normal,
                               const float3& viewDir, const float3& lightDir);
    static float fresnel(const Material& mat, float cosTheta);
};
```

**Methods:**
- `evaluateBRDF()` - Evaluate GGX BRDF
- `fresnel()` - Compute Fresnel term

---

## Lighting

### Light

```cpp
struct Light {
    float3 position;    // World position
    float3 color;       // Light color (0-1)
    float intensity;    // Intensity multiplier
    int type;           // 0=point, 1=directional, 2=area
};
```

**Types:**
- `0` - Point light (omnidirectional)
- `1` - Directional light (infinite distance)
- `2` - Area light (not yet implemented)

**Example Usage:**
```cpp
Light mainLight;
mainLight.position = {5.0f, 8.0f, 5.0f};
mainLight.color = {1.0f, 1.0f, 1.0f};
mainLight.intensity = 1.5f;
mainLight.type = 0;  // Point light

scene->addLight(mainLight);
```

---

## Math Types

### Vector Types

```cpp
using float2 = glm::vec2;  // 2D vector
using float3 = glm::vec3;  // 3D vector
using float4 = glm::vec4;  // 4D vector
using mat4 = glm::mat4;    // 4x4 matrix
```

### Ray

```cpp
struct Ray {
    float3 origin;      // Ray origin
    float3 direction;   // Ray direction (normalized)
    float tMin;         // Minimum t value (default 0.0001)
    float tMax;         // Maximum t value (default 1e30)
};
```

### Intersection

```cpp
struct Intersection {
    bool hit;           // Was hit?
    float distance;     // Distance to hit
    float3 position;    // Hit position
    float3 normal;      // Surface normal
    float2 texCoord;    // UV coordinates
    int triangleID;     // Triangle index
    int materialID;     // Material index
};
```

---

## Utility Functions

### Logger

```cpp
class Logger
{
public:
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void setLogLevel(Level level);
};
```

**Example:**
```cpp
Logger::info("Plugin initialized");
Logger::warning("Performance warning");
Logger::error("Critical error");
```

### Timer

```cpp
class Timer
{
public:
    Timer(const std::string& name = "Timer");
    double elapsed() const;  // Get elapsed time in ms
    void reset();            // Reset timer
};
```

**Example:**
```cpp
{
    Timer timer("Frame Time");  // Prints elapsed time on destruction
    // ... do work ...
}
```

---

## Constants and Settings

### Performance Tuning

```cpp
// Max ray bounces (increase for better GI, slower)
engine->setMaxBounces(2);  // Default: 2

// Samples per pixel (increase for quality, slower)
engine->setSamplesPerPixel(1);  // Default: 1

// Temporal accumulation (recommended: enabled)
engine->setAccumulationEnabled(true);  // Default: true
```

### GPU Limits

- Maximum scene size: Limited by GPU VRAM
- Recommended: 4GB+ for interactive rates
- Minimum compute capability: 7.0 (RTX/Volta)

---

## Error Handling

All functions return status codes:
- `true` / `MStatus::kSuccess` - Operation succeeded
- `false` / `MStatus::kFailure` - Operation failed

**Example:**
```cpp
if (!engine->initialize()) {
    Logger::error("Engine initialization failed");
    return;
}
```

---

## Threading Model

- **CPU Thread:** Main Maya thread handles scene updates and frame submission
- **GPU Threads:** CUDA kernels run on GPU (1024 threads per block, multiple blocks)

**Important:** Do not call GPU functions from non-main threads without synchronization.

---

## Version Information

- **Plugin Version:** 0.1.0 (Prototype)
- **Maya Versions:** 2020+
- **CUDA Toolkit:** 11.0+
- **GPU Requirement:** Compute Capability 7.0+ (RTX/Volta or newer)

---

## See Also

- [Build Instructions](BUILD.md)
- [Architecture Documentation](ARCHITECTURE.md)
- [CUDA Setup Guide](CUDA_SETUP.md)
- [Quick Start Guide](QUICKSTART.md)
