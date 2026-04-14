#pragma once

#include "mesh.hpp"
#include <string>
#include <glm/glm.hpp>

// Will load an .obj file living at @filepath, should a valid .obj file live there.
// The file's contents are translated to a compact in-memory representation and placed
// in the inputted Mesh object.
void load_obj(const std::string& filepath, Mesh& mesh);
