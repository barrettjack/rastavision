#include "types_and_utils.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <glm/common.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

void apply_fragment_shader(const FragmentData& fragments,
                           uint32_t* RGBA_values,
                           const DisplayInfo display_info,
                           const Camera& camera) {
    for (uint32_t y = 0; y < display_info.ny; ++y) {
        for (uint32_t x = 0; x < display_info.nx; ++x) {
            // get the fragment at (x, y)
            // compute the colour of the fragment according to the Phong reflection/material model
            // (ambient + diffuse + specular = Phong reflection)

            // Materials and Lights must be set beyond this point.
            assert(!Material::K_d.empty() and !Material::K_s.empty() and !Material::alpha.empty());
            assert(!Light::directions.empty() and !Light::intensities.empty());

            const uint32_t fragment_index = x + display_info.nx * y;
            const Fragment& fragment = fragments.fragments[fragment_index];

            // Computing the vector oriented towards the camera:
            glm::vec3 pos_on_primitive {fragment.pwx, fragment.pwy, fragment.pwz};
            glm::vec3 dir_to_camera = glm::normalize(camera.position - pos_on_primitive);

            // Computing the vector oriented towards the origin of a light source.
            // For now, just assuming that one light exists.
            glm::vec3 dir_to_light = -Light::directions[0];

            // Computing the perfect reflection direction:
            glm::vec3 normal {fragment.nx, fragment.ny, fragment.nz};
            float cos_theta = glm::dot(dir_to_light, normal);
            glm::vec3 perfect_reflection_dir = ((2 * cos_theta) * normal) - dir_to_light;

            // Finally, computing the fragment's colour.
            // For now, assuming only one light, thus only one intensity.
            float cos_theta_clamped = glm::max(0.0f, cos_theta);
            // TODO(jack): break this monstrosity up. The trick with 1 in the denominator makes me ill.
            glm::vec3 colour = (Light::intensities[0] * cos_theta_clamped) * (Material::K_d[0] + (Material::K_s[0] * glm::pow(cos_theta_clamped, Material::alpha[0])/(0.0f ? 1.0f : cos_theta) ));

            // Colours are quantized to the range [0, 255]
            uint8_t r = static_cast<uint8_t>(std::clamp(colour.x, 0.0f, 1.0f) * 255.0f);
            uint8_t g = static_cast<uint8_t>(std::clamp(colour.y, 0.0f, 1.0f) * 255.0f);
            uint8_t b = static_cast<uint8_t>(std::clamp(colour.z, 0.0f, 1.0f) * 255.0f);

            // We can now assign to the buffer of RGBA values:
            // A placeholder maxed-out alpha (no transparancy implemented) is used:
            uint8_t alpha = 255;
            RGBA_values[fragment_index] = (r << 24) | (g << 16) | (b << 8) | alpha;
        }
    }
}
