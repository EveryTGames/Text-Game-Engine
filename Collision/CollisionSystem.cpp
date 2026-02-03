#include "CollisionSystem.h"

bool CollisionSystem::pixelPerfectCollisionAt(
    std::shared_ptr<Sprite> sprite,
    int localX, int localY,
    const Map *map,
    int mapX, int mapY)
{
    if (!sprite || !map)
        return false;

    // Check sprite pixel solidity
    if (sprite->isSolid(localX, localY) == 0)
        return false;

    // Check map pixel solidity
    if (!map->isSolid(mapX, mapY))
        return false;

    return true; // collision detected
}

bool CollisionSystem::isBoxCollidingWithMap(
    const Map *map,
    int x, int y,
    int width, int height,
    std::shared_ptr<Sprite> sprite,
    std::vector<CollisionNS::CollisionFlag> &outFlags)
{
    bool collisionFound = false;
    outFlags.clear();

    if (!map || !sprite)
        return false;

    // Check only the boundary pixels of the bounding box
    for (int px = x; px < x + width; px++)
    {
        for (int py = y; py < y + height; py++)
        {
            // Skip pixels inside the boundary (check only top, bottom, left, right edges)
            bool onLeftEdge = (px == x);
            bool onRightEdge = (px == x + width - 1);
            bool onTopEdge = (py == y);
            bool onBottomEdge = (py == y + height - 1);

            if (!(onLeftEdge || onRightEdge || onTopEdge || onBottomEdge))
                continue;

            // Check if map pixel solid
            if (map->isSolid(px, py))
            {
                // Translate map pixel to sprite local coordinates
                int localX = px - x;
                int localY = py - y;

                // Check sprite pixel alpha
                if (sprite->isSolid(localX, localY) > 0)
                {
                    // Solid pixel collision: immediate collision
                    collisionFound = true;
                }
                else
                {
                    // Sprite pixel transparent but map solid: possible near-collision
                    outFlags.push_back({px, py});
                }
            }
        }
    }

    return collisionFound;
}
