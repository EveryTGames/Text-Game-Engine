#include "Map.h"
#include <iostream>
#include "../Utils/PixelTypes.h"
#include "../Utils/stb_image.h" 
#include "../ImageLoader/ImageLoader.h"

RGBA Map::getPixelAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height)
        return RGBA{0,0,0,255}; // Return empty tile
    return tiles[y][x];
}

bool Map::loadFromFile(const std::string& filename) {
   std::vector<std::vector<RGBA>> mapPixels;
    int w, h;
    if (ImageLoader::loadImage(filename, mapPixels, w, h)) {
        width = w;
        height = h;
        tiles = mapPixels; 
        return true;
    }
    else {
        std::cerr << "Failed to load map from file: " << filename << std::endl;
        return false;
    }
}

RenderResult Map::getRenderData(const Camera& camera) const {
    RenderResult result;
    result.visible = true; // Map is always partially or fully visible in camera view

    // Determine visible bounds in map coordinates
    int startX = std::max(camera.position.x, 0);
    int startY = std::max(camera.position.y, 0);
    int endX = std::min(camera.position.x + camera.getWidth(), width);
    int endY = std::min(camera.position.y + camera.getHeight(), height);

    // Calculate how much of the map appears on screen
    result.screenX = startX - camera.position.x;
    result.screenY = startY - camera.position.y;
    result.width = endX - startX;
    result.height = endY - startY;

    // Prepare pixel data
    result.visiblePixels.resize(result.height, std::vector<RGBA>(result.width));

    for (int y = 0; y < result.height; ++y) {
        for (int x = 0; x < result.width; ++x) {
            int mapX = startX + x;
            int mapY = startY + y;
            result.visiblePixels[y][x] = getPixelAt(mapX, mapY);
        }
    }

    return result;
}

bool Map::isSolid(int x, int y) const {
    PixelType type = classifyPixelAlpha(getPixelAt(x, y).a);
    return type == PixelType::Solid;
}

bool Map::isTrigger(int x, int y) const {
    PixelType type = classifyPixelAlpha(getPixelAt(x, y).a);
    return type == PixelType::Trigger;
}
