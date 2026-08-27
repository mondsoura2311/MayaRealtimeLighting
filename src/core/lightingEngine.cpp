#include "lightingEngine.h"
#include "scene.h"
#include "../utils/logger.h"
#include <cuda_runtime.h>

LightingEngine::LightingEngine()
    : m_cudaContext(nullptr), m_rayBuffer(nullptr),
      m_accumulationBuffer(nullptr), m_maxBounces(2),
      m_samplesPerPixel(1), m_accumulationEnabled(true), m_frameCount(0)
{
    Logger::info("LightingEngine constructor called");
}

LightingEngine::~LightingEngine()
{
    Logger::info("LightingEngine destructor called");
    shutdown();
}

bool LightingEngine::initialize()
{
    Logger::info("Initializing LightingEngine");
    
    // Initialize CUDA
    if (!initializeCUDA()) {
        Logger::error("CUDA initialization failed");
        return false;
    }
    
    // Initialize scene
    if (!initializeScene()) {
        Logger::error("Scene initialization failed");
        return false;
    }
    
    Logger::info("LightingEngine initialized successfully");
    return true;
}

bool LightingEngine::initializeCUDA()
{
    // Check for CUDA-capable GPU
    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);
    
    if (err != cudaSuccess || deviceCount == 0) {
        Logger::error("No CUDA-capable GPU found");
        return false;
    }
    
    // Get device properties
    cudaDeviceProp deviceProp;
    err = cudaGetDeviceProperties(&deviceProp, 0);
    if (err != cudaSuccess) {
        Logger::error("Failed to get GPU properties");
        return false;
    }
    
    // Check compute capability (minimum 7.0 for RTX)
    if (deviceProp.major < 7) {
        Logger::error("GPU compute capability < 7.0, RTX required");
        return false;
    }
    
    Logger::info("CUDA GPU found: " + std::string(deviceProp.name));
    Logger::info("Compute Capability: " + std::to_string(deviceProp.major) + "." + std::to_string(deviceProp.minor));
    Logger::info("Total Memory: " + std::to_string(deviceProp.totalGlobalMem / (1024 * 1024)) + " MB");
    
    // Set device
    err = cudaSetDevice(0);
    if (err != cudaSuccess) {
        Logger::error("Failed to set CUDA device");
        return false;
    }
    
    // Allocate device buffers
    // TODO: Allocate actual buffers once scene size is known
    
    Logger::info("CUDA initialized successfully");
    return true;
}

bool LightingEngine::initializeScene()
{
    m_scene = std::make_unique<Scene>();
    if (!m_scene->initialize()) {
        Logger::error("Failed to initialize scene");
        return false;
    }
    
    Logger::info("Scene initialized");
    return true;
}

void LightingEngine::shutdown()
{
    Logger::info("Shutting down LightingEngine");
    cleanupCUDA();
    
    if (m_scene) {
        m_scene->shutdown();
        m_scene.reset();
    }
}

void LightingEngine::cleanupCUDA()
{
    if (m_accumulationBuffer != nullptr) {
        cudaFree(m_accumulationBuffer);
        m_accumulationBuffer = nullptr;
    }
    
    if (m_rayBuffer != nullptr) {
        cudaFree(m_rayBuffer);
        m_rayBuffer = nullptr;
    }
    
    Logger::info("CUDA resources cleaned up");
}

void LightingEngine::updateScene()
{
    if (m_scene) {
        m_scene->update();
    }
}

void LightingEngine::traceFrame(float4* outputBuffer, uint32_t width, uint32_t height, uint32_t sampleCount)
{
    if (!m_scene) return;
    
    // TODO: Implement CUDA ray tracing kernel launch
    // This will trace rays and accumulate results
    
    m_frameCount += sampleCount;
}

void LightingEngine::resetAccumulation()
{
    m_frameCount = 0;
    // TODO: Clear accumulation buffer on GPU
    Logger::info("Accumulation buffer reset");
}
