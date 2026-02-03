#pragma once
#include <string>
#include "../Behaviores/BehaviorManager.h"
#include "../Behaviores/IBehaviorAttachable.h"
#include "../ImageLoader/ImageLoader.h"
#include "../Behaviores/BehaviorAttachableBase.h"

class Sprite :  public BehaviorAttachableBase<Sprite>
{
    DECLARE_BEHAVIOR_ATTACHABLE(Sprite,
    (const std::string& imagePath, int maxWidth, int maxHeight),
    { loadImage(imagePath, maxWidth, maxHeight); }
)
public:
    
    int getWidth() const;
    int getHeight() const;
    RGBA getPixel(int x, int y) const;
    bool isSolid(int x, int y) const;
    bool isTrigger(int x, int y) const;

    std::vector<std::vector<RGBA>> getPartialPixels(int startX, int startY, int w, int h) const;

    std::vector<std::vector<RGBA>> getPixels()
    {
        return pixels;
    }

    void setPixels(std::vector<std::vector<RGBA>> newPixels)
    {
        pixels = std::move(newPixels);
        height = static_cast<int>(pixels.size());
        width = (height > 0) ? static_cast<int>(pixels[0].size()) : 0;
    }

private:
    int width, height;
    std::vector<std::vector<RGBA>> pixels;

    void loadImage(const std::string &path, int maxWidth, int maxHeight);
};
