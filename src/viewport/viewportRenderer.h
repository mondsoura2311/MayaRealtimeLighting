#pragma once

#include "../utils/math.h"
#include <vector>

class ViewportRenderer
{
public:
    ViewportRenderer();
    ~ViewportRenderer();
    
    // Lifecycle
    bool initialize();
    void shutdown();
    
    // Rendering control
    void enableRendering();
    void disableRendering();
    bool isRenderingEnabled() const { return m_renderingEnabled; }
    
    // Render frame
    void render();
    
    // Viewport dimensions
    void setViewportSize(int width, int height);
    int getViewportWidth() const { return m_width; }
    int getViewportHeight() const { return m_height; }
    
    // Camera setup
    void setCameraMatrix(const mat4& view, const mat4& projection);
    const mat4& getViewMatrix() const { return m_viewMatrix; }
    const mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    
    // Get output texture
    uint32_t getOutputTextureID() const { return m_outputTexture; }
    
private:
    bool initializeOpenGL();
    bool initializeShaders();
    void cleanupOpenGL();
    
    uint32_t m_framebuffer;
    uint32_t m_outputTexture;
    uint32_t m_depthTexture;
    uint32_t m_shaderProgram;
    uint32_t m_quadVAO;
    uint32_t m_quadVBO;
    
    int m_width;
    int m_height;
    bool m_renderingEnabled;
    
    mat4 m_viewMatrix;
    mat4 m_projectionMatrix;
};
