#pragma once

#include <maya/MFnPlugin.h>
#include <maya/MViewport2Renderer.h>
#include <memory>

class ViewportRenderer;
class LightingEngine;

class MayaRealtimeLightingPlugin
{
public:
    MayaRealtimeLightingPlugin();
    ~MayaRealtimeLightingPlugin();
    
    // Lifecycle
    MStatus initialize(MFnPlugin& fnPlugin);
    MStatus shutdown();
    
    // Accessors
    ViewportRenderer* getViewportRenderer() const { return m_viewportRenderer.get(); }
    LightingEngine* getLightingEngine() const { return m_lightingEngine.get(); }
    
    // Enable/disable real-time lighting
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
private:
    MStatus registerCommands(MFnPlugin& fnPlugin);
    MStatus deregisterCommands(MFnPlugin& fnPlugin);
    
    std::unique_ptr<ViewportRenderer> m_viewportRenderer;
    std::unique_ptr<LightingEngine> m_lightingEngine;
    bool m_enabled;
};
