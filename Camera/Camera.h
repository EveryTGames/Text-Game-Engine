#pragma once
#include <../Utils/Utils.h>

class Camera
{
public:
    Vec2 position;

    // Static function to get the single instance
    static Camera &GetInstance()
    {
        static Camera instance; // Created once on first call
        return instance;
    }

    // Sets the camera's viewport size
    void setSize(int w, int h)
    {
        width = w;
        height = h;
    }

    bool isInView(Vec2 topLeft, Vec2 size) const
    {
        return !(topLeft.x + size.x < position.x ||
                 topLeft.x > position.x + width ||
                 topLeft.y + size.y < position.y ||
                 topLeft.y > position.y + height);
    }

    Vec2 worldToCamera(Vec2 worldPos) const
    {
        return {worldPos.x - position.x, worldPos.y - position.y};
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int width, height;
};
