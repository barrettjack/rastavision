#pragma once

#include "camera.hpp"
#include "model.hpp"
#include "display_info.hpp"
#include <cstdint>
#include <glm/glm.hpp>

// goals for the vertex shader:
//
// take in a vertex and, through a sequence of transformations, obtain its
// coordinates in the canonical view volume.
//
// additionally, the vertex's normal should be transformed IN WORLD SPACE
// i.e. it does not undergo the same sequence of transformations as vertex
// position information
//
// texture coordinates are ignored

typedef struct {
    float px, py, pz;
    float nx, ny, nz;
    float pwx, pwy, pwz; // world-space position data for a model's vertex is retained
                         // for downstream processing in the rasterizer and fragment
                         // shader
} ScreenSpaceVertex;

typedef struct {
    ScreenSpaceVertex* vertices;
    uint32_t* indices;
    uint32_t vertex_count;
    uint32_t index_count;
} ScreenSpaceData;

// Notes to caller:
// 1. This function allocates model.mesh.vertex_count number of ScreenSpaceVertex objects.
// 2. For the world coordinate system, a right-handed system with +z up is assumed.
//
// Transforms all of model's vertices to screen space and transforms model's normal
// vectors into world space.
ScreenSpaceData apply_vertex_shader(const Model& model, const Camera& camera, const DisplayInfo& display_info);
