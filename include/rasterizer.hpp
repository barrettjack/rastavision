
#include "vertex_shader.hpp"
#include <cstdint>

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

void rasterizer(const ScreenSpaceData& vertex_shader_outputs,
                FragmentData& fragments,
                z_buffer z_buffer,
                const DisplayInfo display_info);
