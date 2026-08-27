#include "bvh.h"
#include "../utils/logger.h"
#include <algorithm>
#include <numeric>

BVH::BVH() : m_built(false)
{
    Logger::debug("BVH constructor called");
}

BVH::~BVH()
{
    Logger::debug("BVH destructor called");
}

void BVH::build(const std::vector<Triangle>& triangles)
{
    if (triangles.empty()) {
        Logger::warning("Cannot build BVH: empty triangle list");
        return;
    }
    
    Logger::info("Building BVH for " + std::to_string(triangles.size()) + " triangles");
    
    m_triangles = triangles;
    m_nodes.clear();
    
    // Build recursive structure
    buildRecursive(m_triangles, 0, m_triangles.size(), 0);
    
    m_built = true;
    Logger::info("BVH built successfully with " + std::to_string(m_nodes.size()) + " nodes");
}

int BVH::buildRecursive(const std::vector<Triangle>& triangles,
                        int triangleStart, int triangleCount, int depth)
{
    if (triangleCount <= 0) return -1;
    
    BVHNode node;
    node.triangleStart = -1;
    node.leftChild = -1;
    node.rightChild = -1;
    node.triangleCount = triangleCount;
    
    // Compute bounding box
    node.bbox = BBox();
    for (int i = triangleStart; i < triangleStart + triangleCount; ++i) {
        const Triangle& tri = triangles[i];
        node.bbox.expand(tri.v0);
        node.bbox.expand(tri.v1);
        node.bbox.expand(tri.v2);
    }
    
    // Create leaf if small enough or depth limit reached
    if (triangleCount <= 4 || depth > 30) {
        node.triangleStart = triangleStart;
        int nodeIdx = m_nodes.size();
        m_nodes.push_back(node);
        return nodeIdx;
    }
    
    // Find best split axis
    float3 extent = node.bbox.extents();
    int splitAxis = 0;
    if (extent.y > extent.x) splitAxis = 1;
    if (extent.z > extent[splitAxis]) splitAxis = 2;
    
    // Sort triangles along split axis
    float splitPos = node.bbox.center()[splitAxis];
    
    auto compareFn = [&triangles, splitAxis, splitPos](const int& a, const int& b) {
        float centerA = (triangles[a].v0[splitAxis] + triangles[a].v1[splitAxis] + triangles[a].v2[splitAxis]) / 3.0f;
        float centerB = (triangles[b].v0[splitAxis] + triangles[b].v1[splitAxis] + triangles[b].v2[splitAxis]) / 3.0f;
        return centerA < centerB;
    };
    
    // Partition triangles
    int splitIdx = triangleStart;
    for (int i = triangleStart; i < triangleStart + triangleCount; ++i) {
        float center = (triangles[i].v0[splitAxis] + triangles[i].v1[splitAxis] + triangles[i].v2[splitAxis]) / 3.0f;
        if (center < splitPos) {
            std::swap(m_triangles[i], m_triangles[splitIdx]);
            splitIdx++;
        }
    }
    
    // Ensure proper split
    if (splitIdx == triangleStart || splitIdx == triangleStart + triangleCount) {
        splitIdx = triangleStart + triangleCount / 2;
    }
    
    // Recursively build children
    node.leftChild = buildRecursive(m_triangles, triangleStart, splitIdx - triangleStart, depth + 1);
    node.rightChild = buildRecursive(m_triangles, splitIdx, triangleStart + triangleCount - splitIdx, depth + 1);
    
    int nodeIdx = m_nodes.size();
    m_nodes.push_back(node);
    return nodeIdx;
}

bool BVH::raycast(const Ray& ray, Intersection& intersection) const
{
    if (!m_built || m_nodes.empty()) return false;
    return raycastRecursive(0, ray, intersection);
}

bool BVH::raycastRecursive(int nodeIdx, const Ray& ray, Intersection& intersection) const
{
    if (nodeIdx < 0 || nodeIdx >= m_nodes.size()) return false;
    
    const BVHNode& node = m_nodes[nodeIdx];
    
    // Check bounding box intersection
    // Simple AABB test
    float tMin = ray.tMin, tMax = ray.tMax;
    
    for (int i = 0; i < 3; ++i) {
        float t1 = (node.bbox.min[i] - ray.origin[i]) / (ray.direction[i] + 1e-6f);
        float t2 = (node.bbox.max[i] - ray.origin[i]) / (ray.direction[i] + 1e-6f);
        
        if (ray.direction[i] < 0) std::swap(t1, t2);
        
        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
    }
    
    if (tMin > tMax) return false;
    
    bool hitFound = false;
    
    // Leaf node - test triangles
    if (node.triangleStart >= 0) {
        for (int i = node.triangleStart; i < node.triangleStart + node.triangleCount; ++i) {
            float distance, u, v;
            if (rayTriangleIntersection(ray, m_triangles[i], distance, u, v)) {
                if (distance < intersection.distance) {
                    intersection.distance = distance;
                    intersection.hit = true;
                    intersection.triangleID = i;
                    intersection.position = ray.origin + ray.direction * distance;
                    intersection.materialID = m_triangles[i].materialID;
                    
                    // Compute normal (interpolated)
                    float w = 1.0f - u - v;
                    intersection.normal = glm::normalize(
                        w * m_triangles[i].n0 +
                        u * m_triangles[i].n1 +
                        v * m_triangles[i].n2
                    );
                    
                    hitFound = true;
                }
            }
        }
    } else {
        // Internal node - recurse to children
        if (node.leftChild >= 0) {
            if (raycastRecursive(node.leftChild, ray, intersection)) {
                hitFound = true;
            }
        }
        if (node.rightChild >= 0) {
            if (raycastRecursive(node.rightChild, ray, intersection)) {
                hitFound = true;
            }
        }
    }
    
    return hitFound;
}

bool BVH::rayTriangleIntersection(const Ray& ray, const Triangle& tri,
                                  float& distance, float& u, float& v) const
{
    // Möller-Trumbore algorithm
    const float EPSILON = 1e-6f;
    
    float3 edge1 = tri.v1 - tri.v0;
    float3 edge2 = tri.v2 - tri.v0;
    float3 h = glm::cross(ray.direction, edge2);
    float a = glm::dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON) return false;
    
    float f = 1.0f / a;
    float3 s = ray.origin - tri.v0;
    u = f * glm::dot(s, h);
    
    if (u < 0.0f || u > 1.0f) return false;
    
    float3 q = glm::cross(s, edge1);
    v = f * glm::dot(ray.direction, q);
    
    if (v < 0.0f || u + v > 1.0f) return false;
    
    distance = f * glm::dot(edge2, q);
    
    return distance > ray.tMin && distance < ray.tMax;
}
