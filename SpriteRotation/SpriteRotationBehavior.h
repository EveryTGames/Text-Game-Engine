#pragma once
#include "../Behaviores/IBehavior.h"
#include "../Sprite/Sprite.h"
#include "../Utils/RotationUtils.h"
#include <unordered_map>
#include <cmath>

class SpriteRotationBehavior : public IBehavior<Sprite>
{
private:
    std::shared_ptr<Sprite> sprite;
    std::unordered_map<int, std::vector<std::vector<RGBA>>> rotationCache;

public:
    void attach(std::shared_ptr<Sprite> s) override
    {
        sprite = s;
    }

    void setInitialCache(std::vector<std::vector<RGBA>> initialPixels)
    {
        rotationCache[0] = initialPixels;
    }
    int snapTo16Directions(float angleDegrees)
    {
        angleDegrees = fmodf(angleDegrees, 360.0f);
        if (angleDegrees < 0)
            angleDegrees += 360.0f;
        return static_cast<int>(std::round(angleDegrees / 22.5f)) % 16;
    }

    void rotate(float angleDegrees)
    {
        int index = snapTo16Directions(angleDegrees);
        std::vector<std::vector<RGBA>> finalPixels;
        auto it = rotationCache.find(index);
        if (it != rotationCache.end())
        {

            finalPixels = it->second;
        }
        else
        {

            float snappedAngle = index * 22.5f;
            auto rotated = RotationUtils::rotateSpritePixels(rotationCache[0], snappedAngle);
            rotationCache[index] = rotated;
            finalPixels = rotationCache[index];
        }
        
        sprite->setPixels(finalPixels);
    }
};
