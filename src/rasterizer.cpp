#include "../include/rasterizer.hpp"

void rasterizer(const ScreenSpaceData& vertex_shader_outputs, FragmentData& fragments) {
    // Pseudocode:
    //
    // For each primitive, determine the pixels it covers
    //
    // For each pixel that it covers,
    // if the interpolated depth pz at px, py (the pixel's centre) is closer,
    // overwrite the Fragment at the corresponding index of the Fragment*.
    //
    // Questions:
    // how do we determine the pixels covered by a primitive?
    // NOTE: see how much, as an optimization, computing a primitive's bounding box
    // and only considering pixels within the bounding box improves performance.
    //
    // how should we map between a pixel's position and its index in the Fragment*?
    // NOTE: start with the naive thing, then see how much a "Morton code/Z-order curve"
    // or some other suitable pattern improves things
    //
}
