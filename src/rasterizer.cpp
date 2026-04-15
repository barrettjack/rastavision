#include "../include/rasterizer.hpp"
#include <cstdint>
#include <glm/fwd.hpp>


void rasterizer(const ScreenSpaceData& vertex_shader_outputs,
                FragmentData& fragments,
                z_buffer z_buffer,
                const DisplayInfo display_info) {
    // Pseudocode:
    //
    // For each primitive, determine the pixels it covers
    //
    // For each pixel that it covers,
    // if the interpolated depth pz at px, py (the pixel's centre) is closer,
    // overwrite the Fragment at the corresponding index of the Fragment*.
    //
    // Questions:
    // how do we determine the pixels covered by a primitive?
    // NOTE: see how much, as an optimization, computing a primitive's bounding box
    // and only considering pixels within the bounding box improves performance.
    //
    // how should we map between a pixel's position and its index in the Fragment*?
    // NOTE: start with the naive thing, then see how much a "Morton code/Z-order curve"
    // or some other suitable pattern improves things

    // for each primitive...
    for (uint32_t i = 0; i < vertex_shader_outputs.index_count; i += 3) {
        uint32_t i0 = vertex_shader_outputs.indices[i];
        uint32_t i1 = vertex_shader_outputs.indices[i + 1];
        uint32_t i2 = vertex_shader_outputs.indices[i + 2];

        const ScreenSpaceVertex& v0 = vertex_shader_outputs.vertices[i0];
        const ScreenSpaceVertex& v1 = vertex_shader_outputs.vertices[i1];
        const ScreenSpaceVertex& v2 = vertex_shader_outputs.vertices[i2];

        auto f_01 = [&v0, &v1](float x, float y) {
            return ((v0.py - v1.py) * x) + ((v1.px - v0.px) * y) + (v0.px * v1.py) - (v1.px * v0.py);
        };
        auto f_12 = [&v1, &v2](float x, float y) {
            return ((v1.py - v2.py) * x) + ((v2.px - v1.px) * y) + (v1.px * v2.py) - (v2.px * v1.py);
        };
        auto f_20 = [&v2, &v0](float x, float y) {
            return ((v2.py - v0.py) * x) + ((v0.px - v2.px) * y) + (v2.px * v0.py) - (v0.px * v2.py);
        };

        // TODO(jack): should iterate over triangle's axis-aligned bounding rectangle
        // trivial optimization that could save millions of cycles.
        for (float y = 0; y < static_cast<float>(display_info.ny); y += 1.0f) {
            for (float x = 0; x < static_cast<float>(display_info.nx); x += 1.0f) {
                float alpha = f_12(x, y) / f_12(v0.px, v0.py);
                float beta = f_20(x, y) / f_20(v1.px, v1.py);
                float gamma = f_01(x, y) / f_01(v2.px, v2.py);

                if (alpha > 0 and beta > 0 and gamma > 0) {
                    // interpolate pos on triangle.
                    float interpolated_z_coord = (alpha * v0.pz) + (beta * v1.pz) + (gamma * v2.pz);
                    uint32_t pixel_coordinates = y * display_info.nx + x;
                    if (interpolated_z_coord > z_buffer.z_buffer[pixel_coordinates]) {
                        z_buffer.z_buffer[pixel_coordinates] = interpolated_z_coord;

                        // interpolate attributes and write to fragment!
                        // attributes we care about:
                        // position in world space (pw), normals in world space (n)
                        // attributes we don't (?) care about:
                        // x, y positions in screen space, z position in CVV/clip space (p)
                        Fragment& f = fragments.fragments[pixel_coordinates];
                        f.nx  = alpha * v0.nx  + beta * v1.nx  + gamma * v2.nx;
                        f.ny  = alpha * v0.ny  + beta * v1.ny  + gamma * v2.ny;
                        f.nz  = alpha * v0.nz  + beta * v1.nz  + gamma * v2.nz;
                        f.pwx = alpha * v0.pwx + beta * v1.pwx + gamma * v2.pwx;
                        f.pwy = alpha * v0.pwy + beta * v1.pwy + gamma * v2.pwy;
                        f.pwz = alpha * v0.pwz + beta * v1.pwz + gamma * v2.pwz;
                    }
                }
            }
        }


    }
}
