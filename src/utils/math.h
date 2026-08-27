#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;
using int2 = glm::ivec2;
using int3 = glm::ivec3;
using int4 = glm::ivec4;
using mat4 = glm::mat4;
using mat3 = glm::mat3;

// Ray structure for ray tracing
struct Ray {
    float3 origin;
    float3 direction;
    float tMin;
    float tMax;
    
    Ray() : tMin(0.0001f), tMax(1e30f) {}
    Ray(const float3& o, const float3& d) 
        : origin(o), direction(glm::normalize(d)), tMin(0.0001f), tMax(1e30f) {}
};

// Intersection result
struct Intersection {
    bool hit;
    float distance;
    float3 position;
    float3 normal;
    float3 tangent;
    float2 texCoord;
    int triangleID;
    int materialID;
    
    Intersection() : hit(false), distance(1e30f), triangleID(-1), materialID(-1) {}
};

// Bounding box
struct BBox {
    float3 min;
    float3 max;
    
    BBox() : min(1e30f), max(-1e30f) {}
    BBox(const float3& mn, const float3& mx) : min(mn), max(mx) {}
    
    void expand(const float3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
    
    float3 center() const { return (min + max) * 0.5f; }
    float3 extents() const { return (max - min) * 0.5f; }
    float surfaceArea() const {
        float3 e = extents();
        return 2.0f * (e.x * e.y + e.y * e.z + e.z * e.x);
    }
};

// Utility functions
namespace MathUtils {
    inline float radians(float degrees) {
        return degrees * 3.14159265359f / 180.0f;
    }
    
    inline float degrees(float radians) {
        return radians * 180.0f / 3.14159265359f;
    }
    
    inline float3 reflect(const float3& v, const float3& n) {
        return v - 2.0f * glm::dot(v, n) * n;
    }
    
    inline float3 refract(const float3& v, const float3& n, float eta) {
        float c = -glm::dot(v, n);
        float discriminant = 1.0f - eta * eta * (1.0f - c * c);
        if (discriminant < 0.0f) return float3(0.0f);
        return eta * v + (eta * c - std::sqrt(discriminant)) * n;
    }
    
    inline float fresnel(float cosTheta, float eta) {
        float r0 = (1.0f - eta) / (1.0f + eta);
        r0 *= r0;
        return r0 + (1.0f - r0) * std::pow(1.0f - cosTheta, 5.0f);
    }
}
