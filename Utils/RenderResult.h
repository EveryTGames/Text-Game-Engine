#pragma once
#include <vector>
#include "../ImageLoader/ImageLoader.h"

struct RenderResult {
    int screenX, screenY; // position relative to camera
    int width, height;    // visible width/height
    std::vector<std::vector<RGBA>> visiblePixels;
    bool visible = false;
};