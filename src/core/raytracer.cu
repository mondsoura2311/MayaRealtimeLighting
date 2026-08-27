// CUDA Ray Tracing Kernel for Real-time Global Illumination
// This kernel performs ray tracing on GPU for interactive lighting

#include <cuda_runtime.h>
#include <math_constants.h>
#include <curand_kernel.h>

// Forward declarations
struct Ray;
struct Intersection;
struct Material;
struct Light;

// Device constants
__constant__ int d_maxBounces = 2;
__constant__ int d_lightsCount = 0;

// Device memory pointers (will be set at runtime)
__device__ float3* d_vertices = nullptr;
__device__ float3* d_normals = nullptr;
__device__ Material* d_materials = nullptr;
__device__ Light* d_lights = nullptr;
__device__ int* d_triangleIndices = nullptr;

// ============================================================================
// Math Utilities
// ============================================================================

__device__ float3 reflect(float3 v, float3 n)
{
    return v - 2.0f * dot(v, n) * n;
}

__device__ float3 refract(float3 v, float3 n, float eta)
{
    float c = -dot(v, n);
    float disc = 1.0f - eta * eta * (1.0f - c * c);
    if (disc < 0.0f) return make_float3(0.0f);
    return eta * v + (eta * c - sqrtf(disc)) * n;
}

__device__ float fresnel(float cosTheta, float ior)
{
    float r0 = (1.0f - ior) / (1.0f + ior);
    r0 *= r0;
    float f = powf(1.0f - cosTheta, 5.0f);
    return r0 + (1.0f - r0) * f;
}

__device__ float ggxD(float roughness, float cosH)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float cosH2 = cosH * cosH;
    float denom = cosH2 * (a2 - 1.0f) + 1.0f;
    return a2 / (CUDART_PI_F * denom * denom);
}

__device__ float ggxG(float roughness, float cosV, float cosL)
{
    float a = roughness * roughness;
    float g1v = cosV / (cosV * (1.0f - a) + a);
    float g1l = cosL / (cosL * (1.0f - a) + a);
    return g1v * g1l;
}

__device__ float3 evaluateBRDF(const Material& mat, float3 normal,
                              float3 viewDir, float3 lightDir)
{
    float cosL = fmaxf(dot(lightDir, normal), 0.0f);
    if (cosL <= 0.0f) return make_float3(0.0f);
    
    float3 halfVec = normalize(viewDir + lightDir);
    float cosH = fmaxf(dot(halfVec, normal), 0.0f);
    float cosV = fmaxf(dot(viewDir, normal), 0.0f);
    
    // Diffuse
    float3 diffuse = mat.albedo * (1.0f / CUDART_PI_F);
    
    // Specular (GGX)
    float D = ggxD(mat.roughness, cosH);
    float G = ggxG(mat.roughness, cosV, cosL);
    float F = fresnel(cosH, mat.ior);
    
    float3 specular = make_float3(D * G * F / (4.0f * cosV * cosL + 1e-6f));
    
    // Mix based on metallic
    float3 result = mix(diffuse + specular, specular, mat.metallic);
    
    return result * cosL;
}

// ============================================================================
// Random Number Generation
// ============================================================================

__device__ float randomFloat(curandState* state)
{
    return curand_uniform(state);
}

__device__ float3 randomInHemisphere(float3 normal, curandState* state)
{
    float3 rand = make_float3(
        2.0f * randomFloat(state) - 1.0f,
        2.0f * randomFloat(state) - 1.0f,
        2.0f * randomFloat(state) - 1.0f
    );
    
    float3 result = normalize(rand);
    if (dot(result, normal) < 0.0f) {
        result = -result;
    }
    
    return result;
}

// ============================================================================
// Ray-Triangle Intersection (Möller-Trumbore)
// ============================================================================

__device__ bool rayTriangleIntersection(float3 rayOrig, float3 rayDir,
                                       float3 v0, float3 v1, float3 v2,
                                       float& distance, float& u, float& v,
                                       float tMin = 0.0001f, float tMax = 1e30f)
{
    const float EPSILON = 1e-6f;
    
    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;
    float3 h = cross(rayDir, edge2);
    float a = dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON) return false;
    
    float f = 1.0f / a;
    float3 s = rayOrig - v0;
    u = f * dot(s, h);
    
    if (u < 0.0f || u > 1.0f) return false;
    
    float3 q = cross(s, edge1);
    v = f * dot(rayDir, q);
    
    if (v < 0.0f || u + v > 1.0f) return false;
    
    distance = f * dot(edge2, q);
    
    return distance > tMin && distance < tMax;
}

// ============================================================================
// Direct Lighting
// ============================================================================

__device__ float3 sampleDirectLighting(float3 position, float3 normal,
                                       const Material& mat,
                                       const Light* lights, int lightCount,
                                       curandState* randState)
{
    float3 radiance = make_float3(0.0f);
    
    for (int i = 0; i < lightCount; ++i) {
        Light light = lights[i];
        
        // Light direction
        float3 toLight = light.position - position;
        float distance = length(toLight);
        float3 lightDir = normalize(toLight);
        
        // Attenuation
        float attenuation = 1.0f / (distance * distance + 1e-6f);
        
        // Evaluate BRDF
        float3 viewDir = normalize(-position);  // Simplified
        float3 brdf = evaluateBRDF(mat, normal, viewDir, lightDir);
        
        // Accumulate
        radiance += brdf * light.color * light.intensity * attenuation;
    }
    
    return radiance;
}

// ============================================================================
// Main Ray Tracing Kernel
// ============================================================================

__global__ void rayTracingKernel(
    float4* outputBuffer,
    uint32_t width,
    uint32_t height,
    uint32_t sampleIndex,
    float3 cameraPos,
    float3 cameraDir,
    float cameraFOV,
    uint32_t frameCount)
{
    // Thread coordinates
    uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= width || y >= height) return;
    
    uint32_t pixelIdx = y * width + x;
    
    // Initialize random state
    curandState randState;
    curand_init(pixelIdx + frameCount * width * height, 0, 0, &randState);
    
    // Generate primary ray
    float aspect = (float)width / (float)height;
    float tanHalfFOV = tanf(cameraFOV * CUDART_PI_F / 360.0f);
    
    // Normalized device coordinates
    float ndcX = 2.0f * (float)x / (float)width - 1.0f;
    float ndcY = 2.0f * (float)y / (float)height - 1.0f;
    
    // Camera basis vectors
    float3 forward = normalize(cameraDir);
    float3 right = normalize(cross(forward, make_float3(0.0f, 1.0f, 0.0f)));
    float3 up = cross(right, forward);
    
    // Ray direction
    float3 rayDir = normalize(
        forward +
        right * ndcX * aspect * tanHalfFOV +
        up * ndcY * tanHalfFOV
    );
    
    // Trace ray
    float3 radiance = make_float3(0.0f);
    float3 throughput = make_float3(1.0f);
    float3 rayOrig = cameraPos;
    
    for (int bounce = 0; bounce < d_maxBounces; ++bounce) {
        // Check for intersection (stub - would trace through BVH)
        // For now, return sky color
        float skyInfluence = fmaxf(rayDir.y, 0.0f);
        float3 skyColor = make_float3(0.5f, 0.7f, 1.0f) * skyInfluence +
                         make_float3(1.0f, 0.9f, 0.8f) * (1.0f - skyInfluence);
        
        radiance += throughput * skyColor;
        break;  // For prototype, only sky
    }
    
    // Accumulate result
    if (frameCount == 0) {
        outputBuffer[pixelIdx] = make_float4(radiance.x, radiance.y, radiance.z, 1.0f);
    } else {
        float4 prev = outputBuffer[pixelIdx];
        float blend = 1.0f / (float)(frameCount + 1);
        outputBuffer[pixelIdx] = make_float4(
            mix(prev.x, radiance.x, blend),
            mix(prev.y, radiance.y, blend),
            mix(prev.z, radiance.z, blend),
            1.0f
        );
    }
}

// ============================================================================
// Kernel Launch Wrapper
// ============================================================================

extern "C" void launchRayTracingKernel(
    float4* d_outputBuffer,
    uint32_t width,
    uint32_t height,
    uint32_t sampleIndex,
    float3 cameraPos,
    float3 cameraDir,
    float cameraFOV,
    uint32_t frameCount)
{
    dim3 blockSize(16, 16);
    dim3 gridSize((width + 15) / 16, (height + 15) / 16);
    
    rayTracingKernel<<<gridSize, blockSize>>>(
        d_outputBuffer,
        width,
        height,
        sampleIndex,
        cameraPos,
        cameraDir,
        cameraFOV,
        frameCount
    );
}
