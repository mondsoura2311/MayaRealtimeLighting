#pragma once

#include "../utils/math.h"
#include <memory>
#include <vector>

class Scene;
class LightingEngine
{
public:
    LightingEngine();
    ~LightingEngine();
    
    // Lifecycle
    bool initialize();
    void shutdown();
    
    // Scene management
    Scene* getScene() const { return m_scene.get(); }
    
    // Rendering
    void updateScene();
    void traceFrame(float4* outputBuffer, uint32_t width, uint32_t height, uint32_t sampleCount);
    
    // Settings
    void setMaxBounces(int bounces) { m_maxBounces = bounces; }
    int getMaxBounces() const { return m_maxBounces; }
    
    void setSamplesPerPixel(int samples) { m_samplesPerPixel = samples; }
    int getSamplesPerPixel() const { return m_samplesPerPixel; }
    
    void setAccumulationEnabled(bool enabled) { m_accumulationEnabled = enabled; }
    bool isAccumulationEnabled() const { return m_accumulationEnabled; }
    
    void resetAccumulation();
    
private:
    bool initializeCUDA();
    bool initializeScene();
    void cleanupCUDA();
    
    std::unique_ptr<Scene> m_scene;
    
    // CUDA resources
    void* m_cudaContext;
    void* m_rayBuffer;
    void* m_accumulationBuffer;
    
    // Settings
    int m_maxBounces;
    int m_samplesPerPixel;
    bool m_accumulationEnabled;
    uint32_t m_frameCount;
};
