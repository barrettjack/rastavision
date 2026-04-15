#include "../include/fragment_shader.hpp"
#include <cstdint>

void apply_fragment_shader(const FragmentData& fragments,
                           uint32_t* RGBA_values,
                           DisplayInfo display_info) {
    for (uint32_t y = 0; y < display_info.ny; ++y) {
        for (uint32_t x = 0; x < display_info.nx; ++x) {
            // get the fragment at (x, y)
            // compute the colour of the fragment according to the Phong reflection model
            // (ambient + diffuse + specular = Phong reflection)

            // so, we're missing:
            // camera gaze dir
            // light info
            // material info
        }
    }
}
