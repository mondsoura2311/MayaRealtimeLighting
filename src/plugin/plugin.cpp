#include "plugin.h"
#include "../viewport/viewportRenderer.h"
#include "../core/lightingEngine.h"
#include "../utils/logger.h"
#include <maya/MGlobal.h>

MayaRealtimeLightingPlugin::MayaRealtimeLightingPlugin()
    : m_enabled(false)
{
    Logger::info("MayaRealtimeLightingPlugin constructor called");
}

MayaRealtimeLightingPlugin::~MayaRealtimeLightingPlugin()
{
    Logger::info("MayaRealtimeLightingPlugin destructor called");
}

MStatus MayaRealtimeLightingPlugin::initialize(MFnPlugin& fnPlugin)
{
    Logger::info("Initializing MayaRealtimeLightingPlugin");
    
    // Initialize lighting engine
    try {
        m_lightingEngine = std::make_unique<LightingEngine>();
        if (!m_lightingEngine->initialize()) {
            Logger::error("Failed to initialize LightingEngine");
            return MStatus::kFailure;
        }
        Logger::info("LightingEngine initialized successfully");
    }
    catch (const std::exception& e) {
        Logger::error(std::string("LightingEngine exception: ") + e.what());
        return MStatus::kFailure;
    }
    
    // Initialize viewport renderer
    try {
        m_viewportRenderer = std::make_unique<ViewportRenderer>();
        if (!m_viewportRenderer->initialize()) {
            Logger::error("Failed to initialize ViewportRenderer");
            return MStatus::kFailure;
        }
        Logger::info("ViewportRenderer initialized successfully");
    }
    catch (const std::exception& e) {
        Logger::error(std::string("ViewportRenderer exception: ") + e.what());
        return MStatus::kFailure;
    }
    
    // Register commands
    MStatus status = registerCommands(fnPlugin);
    if (!status) {
        Logger::error("Failed to register commands");
        return status;
    }
    
    return MStatus::kSuccess;
}

MStatus MayaRealtimeLightingPlugin::shutdown()
{
    Logger::info("Shutting down MayaRealtimeLightingPlugin");
    
    setEnabled(false);
    
    if (m_viewportRenderer) {
        m_viewportRenderer->shutdown();
        m_viewportRenderer.reset();
    }
    
    if (m_lightingEngine) {
        m_lightingEngine->shutdown();
        m_lightingEngine.reset();
    }
    
    return MStatus::kSuccess;
}

void MayaRealtimeLightingPlugin::setEnabled(bool enabled)
{
    if (enabled == m_enabled) return;
    
    m_enabled = enabled;
    
    if (enabled) {
        Logger::info("Real-time lighting enabled");
        if (m_viewportRenderer) {
            m_viewportRenderer->enableRendering();
        }
    } else {
        Logger::info("Real-time lighting disabled");
        if (m_viewportRenderer) {
            m_viewportRenderer->disableRendering();
        }
    }
}

MStatus MayaRealtimeLightingPlugin::registerCommands(MFnPlugin& fnPlugin)
{
    // TODO: Register MEL commands for enabling/disabling the plugin
    // This will be implemented in a separate file
    return MStatus::kSuccess;
}

MStatus MayaRealtimeLightingPlugin::deregisterCommands(MFnPlugin& fnPlugin)
{
    // TODO: Deregister MEL commands
    return MStatus::kSuccess;
}
