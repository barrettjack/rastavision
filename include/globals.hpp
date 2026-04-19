#pragma once

#include <string_view>
#include <vector>
#include <glm/vec3.hpp>
#include <string_view>


// Light-related information for shading computations
namespace Light {
    extern std::vector<glm::vec3> directions;

    // NOTE: the components of vectors in intensities correspond to dimensionless red,
    // green, and blue intensities, and range in [0, 1].
    extern std::vector<glm::vec3> intensities;
    extern void init(std::string_view);
}

// Material related properties
// NOTE: the components of vectors in K_s, K_d correspond to dimensionless red,
// green, and blue intensities, and range in [0, 1].
namespace Material {
    extern std::vector<glm::vec3> K_s; // Specular highlights' colour
    extern std::vector<glm::vec3> K_d; // Diffuse light emission colour
    extern std::vector<float> alpha; // Shape parameter for specular highlights
    extern void init(); // Placeholder for something more sensible
}
