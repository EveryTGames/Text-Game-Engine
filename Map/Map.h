#pragma once
#include "../Sprite/Sprite.h"
#include "../ImageLoader/ImageLoader.h"
#include "../Utils/RenderResult.h"
#include "../Camera/Camera.h"


class Map {
public:
    Map() : width(0), height(0) {}
    Map(const std::string& filename) { loadFromFile(filename); }

    bool loadFromFile(const std::string& filename);
    RGBA getPixelAt(int x, int y) const ;
    RenderResult getRenderData(const Camera& camera) const;
    bool isSolid(int x, int y) const;
    bool isTrigger(int x, int y) const;

private:
    int width;
    int height;
    std::vector<std::vector<RGBA>> tiles;
};