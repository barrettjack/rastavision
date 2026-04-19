#pragma once

#include "types_and_utils.hpp"
#include <cstdint>

// rasterizer will output its data to the FragmentData object passed by the callee
// code.
//
// It is important that the callee allocate sufficient memory for the FragmentData
// object's Fragment buffer, i.e. if we wish to write an image to a 720p buffer,
// allocate space for 1280x720 Fragment objects.
extern void rasterizer(const ScreenSpaceData& vertex_shader_outputs,
                FragmentData& fragments,
                z_buffer z_buffer,
                const DisplayInfo display_info,
                const uint32_t* index_buffer,
                const uint32_t index_buffer_len);
