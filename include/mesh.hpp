#pragma once

#include <cstdint>

typedef struct {
    float px, py, pz;   // position
    float nx, ny, nz;   // normal
    float u, v;         // texture coord
} Vertex;

typedef struct {
    Vertex* vertices;    // Vec3 pos, Vec3 normal, Vec2 uv — interleaved
    uint32_t* indices;    // every 3 = one triangle
    uint32_t vertex_count;
    uint32_t index_count;
} Mesh;
