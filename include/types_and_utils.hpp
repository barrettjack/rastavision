#pragma once

#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <string_view>

struct DisplayInfo {
    uint32_t nx, ny;
};

struct Vertex {
    float px, py, pz;   // position
    float nx, ny, nz;   // normal
    float u, v;         // texture coord
};

struct VertexBuffer {
    Vertex* buf;
    uint32_t len;
};

struct IndexBuffer {
    uint32_t* buf;
    uint32_t len;
};

#define IS_UNIT_LENGTH(v) (glm::abs(glm::length((v)) - 1.0f) < 1e-5f)
#define IS_VALID_VIEW_VOLUME(vv) ((vv.l < vv.r) and (vv.b < vv.t) and (vv.n > vv.f))

struct ViewVolume{
    // These floats define the camera's orthographic viewing volume following
    // the conventions laid out in Marschner and Shirley's Fundamentals of Com-
    // puter Graphics
    float l, r, n, f, t, b;
};

struct Camera {
    glm::vec3 position;
    glm::vec3 gaze_direction;
    // Remark: the "up" direction that we will use to construct an orthonormal
    // basis centred at the camera's position is taken (implicitly) to be
    // the vector (0, 0, 1) in a right-handed coordinate system where +z is up.

    ViewVolume view_volume;
};

typedef struct {
    float px, py, pz;
    float nx, ny, nz;
    float pwx, pwy, pwz; // world-space position data for a model's vertex is retained
                         // for downstream processing in the rasterizer and fragment
                         // shader
} ScreenSpaceVertex;

typedef struct {
    ScreenSpaceVertex* vertices;
    uint32_t vertex_count;
} ScreenSpaceData;

struct Fragment {
    float nx, ny, nz; // fragment's interpolated world space normal at alpha beta gamma
    float pwx, pwy, pwz; // interpolated world-space coordinates of alpha beta gamma
};

struct FragmentData {
    Fragment* fragments;
    uint32_t fragment_count;
};

struct z_buffer {
    float* z_buffer;
    uint32_t buffer_length;
};

// Light-related information for shading computations
namespace Light {
    extern std::vector<glm::vec3> directions;

    // NOTE: the components of vectors in intensities correspond to dimensionless red,
    // green, and blue intensities, and range in [0, 1].
    extern std::vector<glm::vec3> intensities;
    extern void init(std::string_view);
}

// Material related properties
// NOTE: the components of vectors in K_s, K_d correspond to dimensionless red,
// green, and blue intensities, and range in [0, 1].
namespace Material {
    extern std::vector<glm::vec3> K_s; // Specular highlights' colour
    extern std::vector<glm::vec3> K_d; // Diffuse light emission colour
    extern std::vector<float> alpha; // Shape parameter for specular highlights
    extern void init(); // Placeholder for something more sensible
}
