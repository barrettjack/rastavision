#pragma once

#include <cstdint>

typedef struct {
    float px, py, pz;   // position
    float nx, ny, nz;   // normal
    float u, v;         // texture coord
} Vertex;

inline bool operator==(const Vertex& a, const Vertex& b) {
    return a.px == b.px && a.py == b.py && a.pz == b.pz
        && a.nx == b.nx && a.ny == b.ny && a.nz == b.nz
        && a.u  == b.u  && a.v  == b.v;
}

typedef struct {
    Vertex* vertices;    // Vec3 pos, Vec3 normal, Vec2 uv — interleaved
    uint32_t* indices;    // every 3 = one triangle
    uint32_t vertex_count = 0;
    uint32_t index_count = 0;
} Mesh;
