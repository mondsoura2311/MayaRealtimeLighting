#include "material.h"
#include "../utils/logger.h"
#include <cmath>

MaterialSystem::MaterialSystem()
{
    Logger::debug("MaterialSystem constructor called");
}

MaterialSystem::~MaterialSystem()
{
    Logger::debug("MaterialSystem destructor called");
}

std::shared_ptr<Material> MaterialSystem::createMaterial()
{
    return std::make_shared<Material>();
}

float3 MaterialSystem::evaluateBRDF(const Material& mat, const float3& normal,
                                   const float3& viewDir, const float3& lightDir)
{
    float cosL = glm::clamp(glm::dot(lightDir, normal), 0.0f, 1.0f);
    if (cosL <= 0.0f) return float3(0.0f);
    
    float3 halfVec = glm::normalize(viewDir + lightDir);
    float cosH = glm::clamp(glm::dot(halfVec, normal), 0.0f, 1.0f);
    float cosV = glm::clamp(glm::dot(viewDir, normal), 0.0f, 1.0f);
    
    // Diffuse component
    float3 diffuse = mat.albedo / 3.14159265359f;
    
    // Specular component (GGX)
    float D = ggxD(mat.roughness, cosH);
    float G = ggxG(mat.roughness, cosV, cosL);
    float F = fresnel(mat, cosH);
    
    float3 specular = float3(D * G * F / (4.0f * cosV * cosL + 1e-6f));
    
    // Blend based on metallic
    float3 result = glm::mix(diffuse + specular, specular, mat.metallic);
    
    return result * cosL;
}

float MaterialSystem::fresnel(const Material& mat, float cosTheta)
{
    float f0 = (mat.ior - 1.0f) / (mat.ior + 1.0f);
    f0 *= f0;
    return f0 + (1.0f - f0) * std::pow(1.0f - cosTheta, 5.0f);
}

float MaterialSystem::ggxD(float roughness, float cosH)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float cosH2 = cosH * cosH;
    float denom = cosH2 * (a2 - 1.0f) + 1.0f;
    return a2 / (3.14159265359f * denom * denom + 1e-6f);
}

float MaterialSystem::ggxG(float roughness, float cosV, float cosL)
{
    float a = roughness * roughness;
    float g1v = cosV / (cosV * (1.0f - a) + a + 1e-6f);
    float g1l = cosL / (cosL * (1.0f - a) + a + 1e-6f);
    return g1v * g1l;
}
