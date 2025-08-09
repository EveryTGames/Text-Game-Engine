#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <../Sprite/Sprite.h>
#include "../Entity/Entity.h"
#include "../World/World.h"
#include "../ImageLoader/ImageLoader.h"




class Engine
{
public:
    Engine(int w, int h);
    void clearBuffer();
    void renderPixel(int x, int y, const RGBA &color);
    void prepareFrameString();
    void swapBuffers();
    void renderSprite(const RenderResult& data);
    void renderScene(const WorldRenderData& data);

    void draw();

private:
    int width, height, currentBuffer = 0;
    std::vector<std::vector<RGBA>> buffers[2];
    std::string frameStrings[2];
};