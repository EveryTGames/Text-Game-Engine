#pragma once
#include <vector>
#include <memory>
#include "../Sprite/Sprite.h"
#include "../Map/Map.h"

namespace CollisionNS {
    struct CollisionFlag
    {
        int x, y; // map pixel coordinates where possible collision occurs
    };
}

class CollisionSystem
{
public:
    // Check if a pixel at (localX, localY) in the sprite collides with a solid map pixel at (mapX, mapY)
    static bool pixelPerfectCollisionAt(
        std::shared_ptr<Sprite> sprite,
        int localX, int localY,
        const Map *map,
        int mapX, int mapY);

    // Check bounding box collision with map. Returns true if immediate solid collision found.
    // Also fills outFlags with near-collisions where the map is solid but sprite pixel is transparent.
    static bool isBoxCollidingWithMap(
        const Map *map,
        int x, int y,
        int width, int height,
        std::shared_ptr<Sprite> sprite,
        std::vector<CollisionNS::CollisionFlag> &outFlags);
};
