#pragma once

#include "types_and_utils.hpp"
#include <glm/glm.hpp>

// Notes to caller:
// 1. For the world coordinate system, a right-handed system with +z up is assumed.
// 2. Outputs will be placed in the ScreenSpaceData object supplied by the callee
//    code. It should be ensured by the callee that the object's ScreenSpaceVertex
//    buffer has enough capacity for the vertices in the supplied Model object.
//
// Transforms all of model's vertices to screen space and transforms model's normal
// vectors into world space. Additionally, world-space position data is computed and stored.
extern void apply_vertex_shader(ScreenSpaceData& ssd,
                                const VertexBuffer vertex_buffer,
                                const IndexBuffer index_buffer,
                                const Camera& camera,
                                const glm::mat4& transform,
                                const DisplayInfo display_info);
