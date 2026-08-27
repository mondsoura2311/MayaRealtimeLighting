#pragma once

#include "../utils/math.h"
#include <vector>
#include <memory>

struct BVHNode {
    BBox bbox;           // Bounding box
    int leftChild;       // Index of left child (-1 = internal leaf)
    int rightChild;      // Index of right child
    int triangleStart;   // Start index for leaf nodes
    int triangleCount;   // Number of triangles in leaf
};

class BVH
{
public:
    BVH();
    ~BVH();
    
    // Build BVH from triangles
    void build(const std::vector<Triangle>& triangles);
    
    // Ray casting
    bool raycast(const Ray& ray, Intersection& intersection) const;
    
    // Getters
    const std::vector<BVHNode>& getNodes() const { return m_nodes; }
    const std::vector<Triangle>& getTriangles() const { return m_triangles; }
    bool isBuilt() const { return m_built; }
    
private:
    struct BuildNode {
        BBox bbox;
        int triangleStart;
        int triangleCount;
        std::vector<int> children;  // Indices of child nodes
    };
    
    int buildRecursive(const std::vector<Triangle>& triangles, 
                      int triangleStart, int triangleCount, int depth);
    
    bool raycastRecursive(int nodeIdx, const Ray& ray, Intersection& intersection) const;
    
    bool rayTriangleIntersection(const Ray& ray, const Triangle& tri, 
                                float& distance, float& u, float& v) const;
    
    std::vector<BVHNode> m_nodes;
    std::vector<Triangle> m_triangles;
    bool m_built;
};
