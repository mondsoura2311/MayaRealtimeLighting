#include "scene.h"
#include "../utils/logger.h"

Scene::Scene()
    : m_cameraPos(0.0f, 0.0f, 10.0f),
      m_cameraDir(0.0f, 0.0f, -1.0f),
      m_cameraUp(0.0f, 1.0f, 0.0f),
      m_cameraFOV(45.0f),
      m_dirty(true)
{
    Logger::info("Scene constructor called");
}

Scene::~Scene()
{
    Logger::info("Scene destructor called");
    shutdown();
}

bool Scene::initialize()
{
    Logger::info("Initializing Scene");
    
    // Add default light
    Light defaultLight;
    defaultLight.position = float3(5.0f, 5.0f, 5.0f);
    defaultLight.color = float3(1.0f, 1.0f, 1.0f);
    defaultLight.intensity = 1.0f;
    defaultLight.type = 0; // Point light
    addLight(defaultLight);
    
    Logger::info("Scene initialized with default light");
    return true;
}

void Scene::shutdown()
{
    Logger::info("Shutting down Scene");
    m_meshes.clear();
    m_lights.clear();
    m_materials.clear();
}

void Scene::update()
{
    if (m_dirty) {
        buildBVH();
        m_dirty = false;
    }
}

void Scene::addMesh(std::shared_ptr<Mesh> mesh)
{
    if (mesh) {
        m_meshes.push_back(mesh);
        m_dirty = true;
        Logger::info("Mesh added to scene");
    }
}

void Scene::addLight(const Light& light)
{
    m_lights.push_back(light);
    Logger::info("Light added to scene");
}

void Scene::addMaterial(std::shared_ptr<Material> material)
{
    if (material) {
        m_materials.push_back(material);
        Logger::info("Material added to scene");
    }
}

void Scene::setCameraPosition(const float3& pos)
{
    m_cameraPos = pos;
}

void Scene::setCameraDirection(const float3& dir)
{
    m_cameraDir = glm::normalize(dir);
}

void Scene::setCameraFOV(float fov)
{
    m_cameraFOV = fov;
}

void Scene::buildBVH()
{
    // TODO: Implement BVH construction
    Logger::info("BVH construction (stub)");
}
