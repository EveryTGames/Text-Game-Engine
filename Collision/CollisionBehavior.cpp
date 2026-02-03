#include "CollisionBehavior.h"
#include "../Utils/Debug.h"
#include <algorithm>
#include <cmath>

void CollisionBehavior::addFlagsNoDuplicates(const std::vector<CollisionNS::CollisionFlag> &newFlags)
{
    for (const CollisionNS::CollisionFlag &flag : newFlags)
    {
        bool found = false;
        for (const CollisionNS::CollisionFlag &existingFlag : collisionFlags)
        {
            if (existingFlag.x == flag.x && existingFlag.y == flag.y)
            {
                found = true;
                break;
            }
        }
        if (!found)
            collisionFlags.push_back(flag);
    }
}

// This re-implements the movement handling previously in PlayerMovement, but
// encapsulated in the behavior so any other script or system can use it.
Vec2 CollisionBehavior::moveWithCollision(const Vec2 &currentPos, const Vec2 &moveAmount, const Map *map, std::shared_ptr<Sprite> sprite)
{
    if (!map || !sprite)
        return currentPos;

    Vec2 pos = currentPos;

    int steps = static_cast<int>(std::ceil(std::max(std::abs(moveAmount.x), std::abs(moveAmount.y))));
    if (steps == 0)
        steps = 1;

    Vec2 stepMove = moveAmount / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i)
    {
        bool movedX = false, movedY = false;

        // Try to move on X axis
        Vec2 nextPosX = pos + Vec2{stepMove.x, 0};
        std::vector<CollisionNS::CollisionFlag> newFlagsX;
        bool collisionX = CollisionSystem::isBoxCollidingWithMap(
            map,
            nextPosX.x,
            nextPosX.y,
            sprite->getWidth(),
            sprite->getHeight(),
            sprite,
            newFlagsX);

        bool pixelCollisionX = false;
        std::vector<CollisionNS::CollisionFlag> filteredFlagsX;

        if (!collisionX)
        {
            for (const CollisionNS::CollisionFlag &flag : collisionFlags)
            {
                if (flag.x >= nextPosX.x &&
                    flag.x < nextPosX.x + sprite->getWidth() &&
                    flag.y >= nextPosX.y &&
                    flag.y < nextPosX.y + sprite->getHeight())
                {
                    int localX = flag.x - nextPosX.x;
                    int localY = flag.y - nextPosX.y;

                    if (CollisionSystem::pixelPerfectCollisionAt(sprite, localX, localY, map, flag.x, flag.y))
                    {
                        pixelCollisionX = true;
                        break;
                    }
                    else
                    {
                        filteredFlagsX.push_back(flag);
                    }
                }
            }
        }

        if (!collisionX && !pixelCollisionX)
        {
            pos.x = nextPosX.x;
            collisionFlags = filteredFlagsX;
            addFlagsNoDuplicates(newFlagsX);
            movedX = true;
        }

        // Try to move on Y axis independently
        Vec2 nextPosY = pos + Vec2{0, stepMove.y};
        std::vector<CollisionNS::CollisionFlag> newFlagsY;
        bool collisionY = CollisionSystem::isBoxCollidingWithMap(
            map,
            nextPosY.x,
            nextPosY.y,
            sprite->getWidth(),
            sprite->getHeight(),
            sprite,
            newFlagsY);

        bool pixelCollisionY = false;
        std::vector<CollisionNS::CollisionFlag> filteredFlagsY;

        if (!collisionY)
        {
            for (const CollisionNS::CollisionFlag &flag : collisionFlags)
            {
                if (flag.x >= nextPosY.x &&
                    flag.x < nextPosY.x + sprite->getWidth() &&
                    flag.y >= nextPosY.y &&
                    flag.y < nextPosY.y + sprite->getHeight())
                {
                    int localX = flag.x - nextPosY.x;
                    int localY = flag.y - nextPosY.y;

                    if (CollisionSystem::pixelPerfectCollisionAt(sprite, localX, localY, map, flag.x, flag.y))
                    {
                        pixelCollisionY = true;
                        break;
                    }
                    else
                    {
                        filteredFlagsY.push_back(flag);
                    }
                }
            }
        }

        if (!collisionY && !pixelCollisionY)
        {
            pos.y = nextPosY.y;
            collisionFlags = filteredFlagsY;
            addFlagsNoDuplicates(newFlagsY);
            movedY = true;
        }

        if (!movedX && !movedY)
            break; // blocked
    }

    // cleanup flags outside bounding box
    collisionFlags.erase(
        std::remove_if(
            collisionFlags.begin(),
            collisionFlags.end(),
            [&](const CollisionNS::CollisionFlag &flag)
            {
                return flag.x < pos.x || flag.x >= pos.x + sprite->getWidth() || flag.y < pos.y || flag.y >= pos.y + sprite->getHeight();
            }),
        collisionFlags.end());

    return pos;
}