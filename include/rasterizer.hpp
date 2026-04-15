
#include "vertex_shader.hpp"
#include <cstdint>

struct Fragment {
    float px, py; // pixel coordinates of the fragment
    float pz; // fragment's interpolated depth at position px, py. -1 <= pz <= 1
    float nx, ny, nz; // fragment's interpolated normal at position px, py.
    float pwx, pwy, pwz; // interpolated world-space coordinates of the pixel's centre
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
