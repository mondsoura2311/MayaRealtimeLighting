#pragma once

#include "../utils/math.h"

struct Material {
    float3 albedo;       // Base color
    float3 emission;     // Emissive color
    float metallic;      // 0 = dielectric, 1 = metal
    float roughness;     // Surface roughness (0 = smooth, 1 = rough)
    float ior;           // Index of refraction
    uint32_t flags;      // Material flags
    
    Material()
        : albedo(0.8f), emission(0.0f), metallic(0.0f), roughness(0.5f), ior(1.5f), flags(0)
    {
    }
};

class MaterialSystem
{
public:
    MaterialSystem();
    ~MaterialSystem();
    
    // Material creation
    std::shared_ptr<Material> createMaterial();
    
    // BRDF evaluation
    static float3 evaluateBRDF(const Material& mat, const float3& normal,
                               const float3& viewDir, const float3& lightDir);
    
    // Fresnel term
    static float fresnel(const Material& mat, float cosTheta);
    
private:
    // GGX normal distribution
    static float ggxD(float roughness, float cosH);
    
    // GGX visibility
    static float ggxG(float roughness, float cosV, float cosL);
};
