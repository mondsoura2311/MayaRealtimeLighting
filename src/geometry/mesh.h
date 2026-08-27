#pragma once

#include "../utils/math.h"
#include <vector>
#include <memory>

struct Triangle {
    float3 v0, v1, v2;  // Vertices
    float3 n0, n1, n2;  // Normals
    float2 uv0, uv1, uv2; // Texture coordinates
    int materialID;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();
    
    // Geometry data
    void setVertices(const std::vector<float3>& vertices);
    void setNormals(const std::vector<float3>& normals);
    void setTexCoords(const std::vector<float2>& texCoords);
    void setIndices(const std::vector<uint32_t>& indices);
    void setMaterialID(int materialID);
    
    const std::vector<Triangle>& getTriangles() const { return m_triangles; }
    const BBox& getBoundingBox() const { return m_bbox; }
    int getMaterialID() const { return m_materialID; }
    
    // Build mesh from data
    void build();
    
    // Transform
    void setTransform(const mat4& transform);
    const mat4& getTransform() const { return m_transform; }
    
private:
    void computeBoundingBox();
    void computeNormals();
    
    std::vector<float3> m_vertices;
    std::vector<float3> m_normals;
    std::vector<float2> m_texCoords;
    std::vector<uint32_t> m_indices;
    std::vector<Triangle> m_triangles;
    
    BBox m_bbox;
    mat4 m_transform;
    int m_materialID;
};
