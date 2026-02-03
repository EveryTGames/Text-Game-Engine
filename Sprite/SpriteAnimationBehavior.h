#pragma once
#include "../Behaviores/IBehavior.h"
#include "../Sprite/Sprite.h"
#include "../ImageLoader/ImageLoader.h"
#include "../Utils/Debug.h"
#include <vector>
#include <string>

class SpriteAnimationBehavior : public IBehavior<Sprite>
{
private:
    std::shared_ptr<Sprite> sprite;
    std::vector<std::vector<std::vector<RGBA>>> frames; // frames -> rows -> cols -> RGBA
    float frameDuration = 0.1f; // seconds per frame
    float elapsed = 0.0f;
    size_t currentFrame = 0;
    bool playing = false;
    bool loop = true;

public:
    void attach(std::shared_ptr<Sprite> owner) override { sprite = owner; }

    void setFrameDuration(float seconds) { frameDuration = seconds; }
    void setLooping(bool l) { loop = l; }

    void addFrame(const std::vector<std::vector<RGBA>> &pixels) { frames.push_back(pixels); }

    // Load frame from file; returns false on failure
    bool addFrameFromFile(const std::string &path, int maxW = 0, int maxH = 0)
    {
        std::vector<std::vector<RGBA>> pixels;
        int w, h;
        if (!ImageLoader::loadImage(path, pixels, w, h, maxW, maxH))
            return false;
        frames.push_back(pixels);
        return true;
    }

    // Load multiple frames from a sprite sheet by slicing it into tiles of size (frameW x frameH).
    // Ignores partial tiles if the sheet dimensions are not exact multiples of frame size.
    bool addFramesFromSheet(const std::string &path, int frameW, int frameH, int maxW = 0, int maxH = 0)
    {
        std::vector<std::vector<RGBA>> sheet;
        int w, h;
        if (!ImageLoader::loadImage(path, sheet, w, h, maxW, maxH))
        {
            Debug::Log("SpriteAnimationBehavior: failed to load sprite sheet: " + path);
            return false;
        }

        if (frameW <= 0 || frameH <= 0)
        {
            Debug::Log("SpriteAnimationBehavior: invalid frame size");
            return false;
        }

        int cols = w / frameW;
        int rows = h / frameH;

        if (cols <= 0 || rows <= 0)
        {
            Debug::Log("SpriteAnimationBehavior: sheet too small for given frame size");
            return false;
        }

        if ((w % frameW) != 0 || (h % frameH) != 0)
        {
            Debug::Log("SpriteAnimationBehavior: sheet dimensions not an exact multiple of frame size; partial tiles will be ignored");
        }

        for (int ry = 0; ry < rows; ++ry)
        {
            for (int cx = 0; cx < cols; ++cx)
            {
                std::vector<std::vector<RGBA>> frame(frameH, std::vector<RGBA>(frameW));
                for (int fy = 0; fy < frameH; ++fy)
                {
                    int sy = ry * frameH + fy;
                    for (int fx = 0; fx < frameW; ++fx)
                    {
                        int sx = cx * frameW + fx;
                        frame[fy][fx] = sheet[sy][sx];
                    }
                }
                frames.push_back(frame);
            }
        }

        return true;
    }

    void play()
    {
        if (frames.empty())
            return;
        playing = true;
        sprite->setPixels(frames[currentFrame]);
    }

    void stop() { playing = false; }
    void reset()
    {
        currentFrame = 0;
        elapsed = 0;
        if (!frames.empty())
            sprite->setPixels(frames[0]);
    }

    void update(float dt)
    {
        if (!playing || frames.empty())
            return;

        elapsed += dt;
        while (elapsed >= frameDuration)
        {
            elapsed -= frameDuration;
            currentFrame++;
            if (currentFrame >= frames.size())
            {
                if (loop)
                    currentFrame = 0;
                else
                {
                    currentFrame = frames.size() - 1;
                    playing = false;
                    break;
                }
            }
            sprite->setPixels(frames[currentFrame]);
        }
    }
};
