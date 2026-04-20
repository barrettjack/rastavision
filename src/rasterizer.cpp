#include "rasterizer.hpp"
#include "types_and_utils.hpp"
#include <cstdint>
#include <glm/common.hpp>
#include <glm/fwd.hpp>


void rasterizer(FragmentData& fragments,
                       z_buffer z_buffer,
                       const ScreenSpaceData& vertex_shader_outputs,
                       const DisplayInfo display_info,
                       const IndexBuffer index_buffer) {
    // TODO(jack):
    // see how much a "Morton code/Z-order curve" or some other suitable access
    // pattern improves performance. granted. this optimization would have implic-
    // ations for how we read the buffer too...

    // for each primitive...
    for (uint32_t i = 0; i < index_buffer.len; i += 3) {
        uint32_t i0 = index_buffer.buf[i];
        uint32_t i1 = index_buffer.buf[i + 1];
        uint32_t i2 = index_buffer.buf[i + 2];

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

        // Computing upper and lower bounds on the pixels that enclose the primitive defined by v0..2.
        float x_lb = glm::min(glm::floor(v0.px), glm::min(glm::floor(v1.px), glm::floor(v2.px)));
        float x_ub = glm::max(glm::ceil(v0.px),  glm::max(glm::ceil(v1.px),  glm::ceil(v2.px)));
        float y_lb = glm::min(glm::floor(v0.py), glm::min(glm::floor(v1.py), glm::floor(v2.py)));
        float y_ub = glm::max(glm::ceil(v0.py),  glm::max(glm::ceil(v1.py),  glm::ceil(v2.py)));
        x_lb = glm::max(x_lb, 0.0f);
        x_ub = glm::min(x_ub, static_cast<float>(display_info.nx - 1));
        y_lb = glm::max(y_lb, 0.0f);
        y_ub = glm::min(y_ub, static_cast<float>(display_info.ny - 1));

        for (float y = y_lb; y < y_ub; y += 1.0f) {
            for (float x = x_lb; x < x_ub; x += 1.0f) {

                // TODO(jack):
                // using alpha beta gamma computed from screen space x and y and using to interpolate
                // world-space stuff might be incorrect, esp. when perspective projection is used.
                // have to revisit the math on this.
                float alpha = f_12(x, y) / f_12(v0.px, v0.py);
                float beta = f_20(x, y) / f_20(v1.px, v1.py);
                float gamma = f_01(x, y) / f_01(v2.px, v2.py);

                if (alpha > 0 and beta > 0 and gamma > 0) {
                    // interpolate pos on triangle.
                    float interpolated_z_coord = (alpha * v0.pz) + (beta * v1.pz) + (gamma * v2.pz);
                    uint32_t pixel_coordinates = y * display_info.nx + x;
                    if (interpolated_z_coord > z_buffer.z_buffer[pixel_coordinates]) {
                        z_buffer.z_buffer[pixel_coordinates] = interpolated_z_coord;
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
