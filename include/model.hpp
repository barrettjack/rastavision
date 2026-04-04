#pragma once

#include "mesh.hpp"
#include <glm/ext/matrix_float4x4.hpp>

typedef struct {
    Mesh mesh;
    glm::mat4 transform;
} Model;
