#include "globals.hpp"
#include <fstream>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <sstream>
#include <iostream>
#include <filesystem>

// BEGIN LIGHT-RELATED
std::vector<glm::vec3> Light::directions {};
std::vector<glm::vec3> Light::intensities {};

void Light::init(std::string_view filepath) {
    std::ifstream file(filepath.data());
    if (!file) {
        std::cerr << "Failed to open file at path: " << filepath << "\n";
        std::cerr << "Current working directory is: " << std::filesystem::current_path() << "\n";
        std::cerr << "Exiting\n";
        exit(1);
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        char type;
        float x, y, z;
        ss >> type >> x >> y >> z;

        if (type == 'd')
            directions.push_back(glm::normalize(glm::vec3(x, y, z)));
        else if (type == 'i')
            // TODO: don't normalize here, but instead, clamp components in [0, 1]
            intensities.emplace_back(x, y, z);
    }
}
// END LIGHT-RELATED


// BEGIN MATERIAL-RELATED
std::vector<glm::vec3> Material::K_s {};
std::vector<glm::vec3> Material::K_d {};
std::vector<float> Material::alpha {};

// Placeholder for something more
void Material::init() {
    Material::K_s.push_back({1.0f, 0.0f, 0.0f});
    Material::K_d.push_back({1.0f, 1.0f, 1.0f});
    Material::alpha.push_back(1.0f);
}
// END MATERIAL-RELATED
