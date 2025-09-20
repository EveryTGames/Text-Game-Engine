#include "Sprite.h"
#include <iostream>
#include <../ImageLoader/ImageLoader.h>
#include "../Utils/PixelTypes.h"


void Sprite::loadImage(const std::string &path, int maxW, int maxH)
{
    ImageLoader::loadImage(path, pixels, width, height, maxW, maxH);
    
    
}

int Sprite::getWidth() const { return width; }
int Sprite::getHeight() const { return height; }

RGBA Sprite::getPixel(int x, int y) const
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        return pixels[y][x];
    return {0, 0, 0}; // Return black if out of bounds
}

std::vector<std::vector<RGBA>> Sprite::getPartialPixels(int startX, int startY, int w, int h) const
{
    std::vector<std::vector<RGBA>> partial;
    for (int y = startY; y < startY + h; ++y)
    {
        std::vector<RGBA> row;
        for (int x = startX; x < startX + w; ++x)
        {
            row.push_back(getPixel(x, y));
        }
        partial.push_back(row);
    }
    return partial;
}

bool Sprite::isSolid(int x, int y) const
{
    PixelType type = classifyPixelAlpha(getPixel(x, y).a);
    return type == PixelType::Solid;
}

bool Sprite::isTrigger(int x, int y) const
{
    PixelType type = classifyPixelAlpha(getPixel(x, y).a);
    return type == PixelType::Trigger;
}