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
} ScreenSpaceVertex;

typedef struct {
    ScreenSpaceVertex* vertices;
    uint32_t* indices;
    uint32_t vertex_count;
    uint32_t index_count;
} ScreenSpaceData;

ScreenSpaceData apply_vertex_shader(const Model& model, const Camera& camera, const DisplayInfo& display_info);
