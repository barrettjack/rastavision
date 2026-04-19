#pragma once

#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>

struct DisplayInfo {
    uint32_t nx, ny;
};

struct Vertex {
    float px, py, pz;   // position
    float nx, ny, nz;   // normal
    float u, v;         // texture coord
};

struct Mesh {
    Vertex* vertices;    // Vec3 pos, Vec3 normal, Vec2 uv — interleaved
    uint32_t* indices;    // every 3 = one triangle
    uint32_t vertex_count;
    uint32_t index_count;
};

struct Model {
    Mesh mesh;
    glm::mat4 transform;
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
