#pragma once

#include "display_info.hpp"
#include "rasterizer.hpp"

void apply_fragment_shader(const FragmentData&, uint32_t*, const DisplayInfo, const Camera&);
