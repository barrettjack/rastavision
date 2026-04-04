#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

typedef struct {
    glm::vec3 position;
    glm::vec3 gaze_direction;
    // Remark: the "up" direction that we will use to construct an orthonormal
    // basis centred at the camera's position is taken (implicitly) to be
    // the vector (0, 0, 1) in a right-handed coordinate system where +z is up.

    float l, r, n, f, t, b;
} Camera;
