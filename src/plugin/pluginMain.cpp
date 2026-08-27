#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MViewport2Renderer.h>
#include "plugin.h"

// Plugin instance
MayaRealtimeLightingPlugin* g_plugin = nullptr;

// Plugin initialization
MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin fnPlugin(obj, "MayaRealtimeLighting", "0.1.0", "Any", &status);
    
    if (!status) {
        MGlobal::displayError("Failed to create plugin function set");
        return status;
    }
    
    // Create plugin instance
    g_plugin = new MayaRealtimeLightingPlugin();
    if (!g_plugin) {
        MGlobal::displayError("Failed to allocate plugin memory");
        return MStatus::kFailure;
    }
    
    // Initialize plugin
    status = g_plugin->initialize(fnPlugin);
    if (!status) {
        MGlobal::displayError("Failed to initialize plugin");
        delete g_plugin;
        g_plugin = nullptr;
        return status;
    }
    
    MGlobal::displayInfo("MayaRealtimeLighting plugin loaded successfully");
    return MStatus::kSuccess;
}

// Plugin deinitialization
MStatus uninitializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin fnPlugin(obj);
    
    if (!status) {
        MGlobal::displayError("Failed to create plugin function set");
        return status;
    }
    
    // Deregister commands
    status = fnPlugin.deregisterCommand("rtLightingToggle");
    if (!status) {
        MGlobal::displayWarning("Failed to deregister rtLightingToggle command");
    }
    
    // Cleanup plugin
    if (g_plugin) {
        g_plugin->shutdown();
        delete g_plugin;
        g_plugin = nullptr;
    }
    
    MGlobal::displayInfo("MayaRealtimeLighting plugin unloaded");
    return MStatus::kSuccess;
}
