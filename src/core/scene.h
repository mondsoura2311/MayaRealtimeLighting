#pragma once

#include "../utils/math.h"
#include "../geometry/mesh.h"
#include "../material/material.h"
#include <vector>
#include <memory>

struct Light {
    float3 position;
    float3 color;
    float intensity;
    int type; // 0 = point, 1 = directional, 2 = area
};

class Scene
{
public:
    Scene();
    ~Scene();
    
    bool initialize();
    void shutdown();
    
    void update();
    
    // Geometry
    void addMesh(std::shared_ptr<Mesh> mesh);
    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return m_meshes; }
    
    // Lights
    void addLight(const Light& light);
    const std::vector<Light>& getLights() const { return m_lights; }
    
    // Materials
    void addMaterial(std::shared_ptr<Material> material);
    const std::vector<std::shared_ptr<Material>>& getMaterials() const { return m_materials; }
    
    // Camera
    void setCameraPosition(const float3& pos);
    void setCameraDirection(const float3& dir);
    void setCameraFOV(float fov);
    
    float3 getCameraPosition() const { return m_cameraPos; }
    float3 getCameraDirection() const { return m_cameraDir; }
    float getCameraFOV() const { return m_cameraFOV; }
    
    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty) { m_dirty = dirty; }
    
private:
    void buildBVH();
    
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<Light> m_lights;
    std::vector<std::shared_ptr<Material>> m_materials;
    
    float3 m_cameraPos;
    float3 m_cameraDir;
    float3 m_cameraUp;
    float m_cameraFOV;
    
    bool m_dirty;
};
