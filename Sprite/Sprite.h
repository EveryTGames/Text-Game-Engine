#pragma once
#include <string>
#include "../ImageLoader/ImageLoader.h"

class Sprite
{
public:
    Sprite(const std::string &imagePath, int maxWidth, int maxHeight);

    int getWidth() const;
    int getHeight() const;
    RGBA getPixel(int x, int y) const;
    bool isSolid(int x, int y) const;
    bool isTrigger(int x, int y) const;

    std::vector<std::vector<RGBA>> getPartialPixels(int startX, int startY, int w, int h) const;

private:
    int width, height;
    std::vector<std::vector<RGBA>> pixels;

    void loadImage(const std::string &path, int maxWidth, int maxHeight);
};
