#include "mesh.h"
#include "../utils/logger.h"
#include <glm/glm.hpp>

Mesh::Mesh() : m_materialID(0)
{
    Logger::debug("Mesh constructor called");
}

Mesh::~Mesh()
{
    Logger::debug("Mesh destructor called");
}

void Mesh::setVertices(const std::vector<float3>& vertices)
{
    m_vertices = vertices;
    Logger::info("Mesh vertices set (" + std::to_string(vertices.size()) + " vertices)");
}

void Mesh::setNormals(const std::vector<float3>& normals)
{
    m_normals = normals;
    Logger::info("Mesh normals set (" + std::to_string(normals.size()) + " normals)");
}

void Mesh::setTexCoords(const std::vector<float2>& texCoords)
{
    m_texCoords = texCoords;
    Logger::info("Mesh texture coordinates set (" + std::to_string(texCoords.size()) + " tex coords)");
}

void Mesh::setIndices(const std::vector<uint32_t>& indices)
{
    m_indices = indices;
    Logger::info("Mesh indices set (" + std::to_string(indices.size()) + " indices)");
}

void Mesh::setMaterialID(int materialID)
{
    m_materialID = materialID;
}

void Mesh::setTransform(const mat4& transform)
{
    m_transform = transform;
}

void Mesh::build()
{
    Logger::info("Building mesh");
    
    if (m_vertices.empty() || m_indices.empty()) {
        Logger::error("Cannot build mesh: empty vertices or indices");
        return;
    }
    
    // Compute normals if not provided
    if (m_normals.empty()) {
        computeNormals();
    }
    
    // Create triangles
    m_triangles.clear();
    for (size_t i = 0; i < m_indices.size(); i += 3) {
        Triangle tri;
        
        uint32_t i0 = m_indices[i];
        uint32_t i1 = m_indices[i + 1];
        uint32_t i2 = m_indices[i + 2];
        
        // Vertices
        tri.v0 = m_vertices[i0];
        tri.v1 = m_vertices[i1];
        tri.v2 = m_vertices[i2];
        
        // Normals
        tri.n0 = m_normals.size() > i0 ? m_normals[i0] : float3(0.0f, 1.0f, 0.0f);
        tri.n1 = m_normals.size() > i1 ? m_normals[i1] : float3(0.0f, 1.0f, 0.0f);
        tri.n2 = m_normals.size() > i2 ? m_normals[i2] : float3(0.0f, 1.0f, 0.0f);
        
        // Texture coordinates
        tri.uv0 = m_texCoords.size() > i0 ? m_texCoords[i0] : float2(0.0f, 0.0f);
        tri.uv1 = m_texCoords.size() > i1 ? m_texCoords[i1] : float2(0.0f, 0.0f);
        tri.uv2 = m_texCoords.size() > i2 ? m_texCoords[i2] : float2(0.0f, 0.0f);
        
        tri.materialID = m_materialID;
        
        m_triangles.push_back(tri);
    }
    
    // Compute bounding box
    computeBoundingBox();
    
    Logger::info("Mesh built successfully (" + std::to_string(m_triangles.size()) + " triangles)");
}

void Mesh::computeBoundingBox()
{
    m_bbox = BBox();
    for (const auto& tri : m_triangles) {
        m_bbox.expand(tri.v0);
        m_bbox.expand(tri.v1);
        m_bbox.expand(tri.v2);
    }
}

void Mesh::computeNormals()
{
    Logger::info("Computing mesh normals");
    
    m_normals.clear();
    m_normals.resize(m_vertices.size(), float3(0.0f));
    
    // Accumulate face normals
    for (size_t i = 0; i < m_indices.size(); i += 3) {
        uint32_t i0 = m_indices[i];
        uint32_t i1 = m_indices[i + 1];
        uint32_t i2 = m_indices[i + 2];
        
        float3 v0 = m_vertices[i0];
        float3 v1 = m_vertices[i1];
        float3 v2 = m_vertices[i2];
        
        // Compute face normal
        float3 e1 = v1 - v0;
        float3 e2 = v2 - v0;
        float3 faceNormal = glm::normalize(glm::cross(e1, e2));
        
        m_normals[i0] += faceNormal;
        m_normals[i1] += faceNormal;
        m_normals[i2] += faceNormal;
    }
    
    // Normalize
    for (auto& n : m_normals) {
        if (glm::length(n) > 0.0f) {
            n = glm::normalize(n);
        } else {
            n = float3(0.0f, 1.0f, 0.0f);
        }
    }
}
