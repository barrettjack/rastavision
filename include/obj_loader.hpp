#pragma once

#include "types_and_utils.hpp"
#include <string>
#include <glm/glm.hpp>

// Will load an .obj file living at @filepath, should a valid .obj file live there.
// The file's contents are translated to a compact in-memory representation and placed
// in the inputted Mesh object.
//
// Note: this function allocates memory sufficient to hold the mesh's data in memory.
// If appropriate, the caller should call delete[] on mesh.vertices and mesh.indices.
extern void load_obj(const std::string& filepath, Mesh& mesh);
