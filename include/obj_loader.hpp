#pragma once

#include "types_and_utils.hpp"
#include <string>
#include <glm/glm.hpp>

// Will load an .obj file living at @filepath, should a valid .obj file live there.
// The file's contents are translated to a compact in-memory representation, which
// is stored in the VertexBuffer and IndexBuffer objects supplied by the callee.
//
// Note: this function allocates memory sufficient to hold the mesh's data in memory.
// If appropriate, the caller should call delete[] on the supplied buffers.
extern void load_obj(const std::string& filepath, VertexBuffer&, IndexBuffer&);
