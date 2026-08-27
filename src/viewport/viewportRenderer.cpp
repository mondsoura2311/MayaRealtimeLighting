#include "viewportRenderer.h"
#include "../utils/logger.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

ViewportRenderer::ViewportRenderer()
    : m_framebuffer(0), m_outputTexture(0), m_depthTexture(0),
      m_shaderProgram(0), m_quadVAO(0), m_quadVBO(0),
      m_width(1280), m_height(720), m_renderingEnabled(false)
{
    Logger::info("ViewportRenderer constructor called");
}

ViewportRenderer::~ViewportRenderer()
{
    Logger::info("ViewportRenderer destructor called");
    shutdown();
}

bool ViewportRenderer::initialize()
{
    Logger::info("Initializing ViewportRenderer");
    
    // Initialize GLEW for OpenGL extension loading
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        Logger::error("GLEW initialization failed");
        return false;
    }
    
    Logger::info("OpenGL version: " + std::string((const char*)glGetString(GL_VERSION)));
    Logger::info("GLSL version: " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
    
    // Initialize OpenGL resources
    if (!initializeOpenGL()) {
        Logger::error("OpenGL initialization failed");
        return false;
    }
    
    // Initialize shaders
    if (!initializeShaders()) {
        Logger::error("Shader initialization failed");
        return false;
    }
    
    Logger::info("ViewportRenderer initialized successfully");
    return true;
}

bool ViewportRenderer::initializeOpenGL()
{
    // Create framebuffer object
    glGenFramebuffers(1, &m_framebuffer);
    if (m_framebuffer == 0) {
        Logger::error("Failed to create framebuffer");
        return false;
    }
    
    // Create output texture (RGBA32F for HDR)
    glGenTextures(1, &m_outputTexture);
    glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Create depth texture
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Attach textures to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_outputTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    
    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::error("Framebuffer incomplete");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Create quad for full-screen rendering
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    Logger::info("OpenGL resources initialized");
    return true;
}

bool ViewportRenderer::initializeShaders()
{
    // TODO: Implement shader loading and compilation
    // For now, we'll use a simple pass-through shader
    Logger::info("Shaders initialized (stub)");
    return true;
}

void ViewportRenderer::shutdown()
{
    Logger::info("Shutting down ViewportRenderer");
    cleanupOpenGL();
}

void ViewportRenderer::cleanupOpenGL()
{
    if (m_quadVAO != 0) {
        glDeleteVertexArrays(1, &m_quadVAO);
        m_quadVAO = 0;
    }
    
    if (m_quadVBO != 0) {
        glDeleteBuffers(1, &m_quadVBO);
        m_quadVBO = 0;
    }
    
    if (m_shaderProgram != 0) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
    
    if (m_outputTexture != 0) {
        glDeleteTextures(1, &m_outputTexture);
        m_outputTexture = 0;
    }
    
    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
        m_depthTexture = 0;
    }
    
    if (m_framebuffer != 0) {
        glDeleteFramebuffers(1, &m_framebuffer);
        m_framebuffer = 0;
    }
    
    Logger::info("OpenGL resources cleaned up");
}

void ViewportRenderer::enableRendering()
{
    if (!m_renderingEnabled) {
        m_renderingEnabled = true;
        Logger::info("ViewportRenderer rendering enabled");
    }
}

void ViewportRenderer::disableRendering()
{
    if (m_renderingEnabled) {
        m_renderingEnabled = false;
        Logger::info("ViewportRenderer rendering disabled");
    }
}

void ViewportRenderer::render()
{
    if (!m_renderingEnabled) return;
    
    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    // Clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Viewport
    glViewport(0, 0, m_width, m_height);
    
    // TODO: Render accumulated lighting data
    // For now, just clear the buffer
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ViewportRenderer::setViewportSize(int width, int height)
{
    if (width == m_width && height == m_height) return;
    
    m_width = width;
    m_height = height;
    
    // Recreate textures with new size
    if (m_outputTexture != 0) {
        glBindTexture(GL_TEXTURE_2D, m_outputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    
    if (m_depthTexture != 0) {
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    
    Logger::info("Viewport size changed to " + std::to_string(width) + "x" + std::to_string(height));
}

void ViewportRenderer::setCameraMatrix(const mat4& view, const mat4& projection)
{
    m_viewMatrix = view;
    m_projectionMatrix = projection;
}
