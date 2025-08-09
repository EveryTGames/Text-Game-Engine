#pragma once
#include "Script.h"
#include <iostream>
#include "Entity/Entity.h"
#include "Map/Map.h"
#include "../Utils/Debug.h"
#include <cmath>
#include <vector>
#include <cmath> // for std::round

struct CollisionFlag
{
    int x, y; // map pixel coordinates where possible collision occurs
};

class PlayerMovement : public Script
{

public:
    // Static function to get the single instance
    static PlayerMovement &GetInstance()
    {
        static PlayerMovement instance; // Created once on first call
        return instance;
    }
    World *world = nullptr;

    std::vector<CollisionFlag> collisionFlags; // store flags across frames
    void onStart() override
    {
        // Initialize world reference
        // This assumes the world instance is set before PlayerMovement is used
        world = &World::GetInstance();

        Debug::Log("PlayerMovement script started");
    }

    inline int roundToInt(float val)
    {
        return static_cast<int>(std::round(val));
    }

    void onUpdate(float dt) override
    {

        auto map = entity->getMap();
        auto &sprite = entity->sprite;
         world->camera.position = entity->position - Vec2{world->camera.getWidth() / 2, world->camera.getHeight() / 2}; // Center camera on player
         Debug::Log("Camera position: " + std::to_string(world->camera.position.x) + ", " + std::to_string(world->camera.position.y));
          Debug::Log("camera width and height: " + std::to_string(world->camera.getWidth() / 2) + ", " + std::to_string(world->camera.getHeight() / 2));
        Vec2float inputVelocity{0.0f, 0.0f};

        if (GetAsyncKeyState('W') & 0x8000)
            inputVelocity.y = -1;
        if (GetAsyncKeyState('S') & 0x8000)
            inputVelocity.y = 1;
        if (GetAsyncKeyState('A') & 0x8000)
            inputVelocity.x = -1;
        if (GetAsyncKeyState('D') & 0x8000)
            inputVelocity.x = 1;

        if (inputVelocity.x != 0 && inputVelocity.y != 0)
        {
            float invLen = 1.0f / std::sqrt(inputVelocity.x * inputVelocity.x + inputVelocity.y * inputVelocity.y);
            inputVelocity.x *= invLen;
            inputVelocity.y *= invLen;
        }

        float speed = 3.0f;
        entity->velocity = inputVelocity * speed;

        Vec2 moveAmount = entity->velocity * dt * 30;

        int steps = static_cast<int>(std::ceil(std::max(std::abs(moveAmount.x), std::abs(moveAmount.y))));
        if (steps == 0)
            steps = 1;

        Vec2 stepMove = moveAmount / static_cast<float>(steps);

        for (int i = 0; i < steps; ++i)
        {
            bool movedX = false, movedY = false;

            // Try to move on X axis
            Vec2 nextPosX = entity->position + Vec2{stepMove.x, 0};
            std::vector<CollisionFlag> newFlagsX;
            bool collisionX = isBoxCollidingWithMap(
                map,
                roundToInt(nextPosX.x),
                roundToInt(nextPosX.y),
                entity->getWidth(),
                entity->getHeight(),
                sprite,
                newFlagsX);

            bool pixelCollisionX = false;
            std::vector<CollisionFlag> filteredFlagsX;

            if (!collisionX)
            {
                for (const CollisionFlag &flag : collisionFlags)
                {
                    if (flag.x >= roundToInt(nextPosX.x) &&
                        flag.x < roundToInt(nextPosX.x) + entity->getWidth() &&
                        flag.y >= roundToInt(nextPosX.y) &&
                        flag.y < roundToInt(nextPosX.y) + entity->getHeight())
                    {
                        int localX = flag.x - roundToInt(nextPosX.x);
                        int localY = flag.y - roundToInt(nextPosX.y);

                        if (pixelPerfectCollisionAt(sprite, localX, localY, map, flag.x, flag.y))
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
                entity->position.x = nextPosX.x;
                collisionFlags = filteredFlagsX;
                collisionFlags.insert(collisionFlags.end(), newFlagsX.begin(), newFlagsX.end());
                movedX = true;
            }

            // Try to move on Y axis independently (even if X movement blocked)
            Vec2 nextPosY = entity->position + Vec2{0, stepMove.y};
            std::vector<CollisionFlag> newFlagsY;
            bool collisionY = isBoxCollidingWithMap(
                map,
                roundToInt(nextPosY.x),
                roundToInt(nextPosY.y),
                entity->getWidth(),
                entity->getHeight(),
                sprite,
                newFlagsY);

            bool pixelCollisionY = false;
            std::vector<CollisionFlag> filteredFlagsY;

            if (!collisionY)
            {
                for (const CollisionFlag &flag : collisionFlags)
                {
                    if (flag.x >= roundToInt(nextPosY.x) &&
                        flag.x < roundToInt(nextPosY.x) + entity->getWidth() &&
                        flag.y >= roundToInt(nextPosY.y) &&
                        flag.y < roundToInt(nextPosY.y) + entity->getHeight())
                    {
                        int localX = flag.x - roundToInt(nextPosY.x);
                        int localY = flag.y - roundToInt(nextPosY.y);

                        if (pixelPerfectCollisionAt(sprite, localX, localY, map, flag.x, flag.y))
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
                entity->position.y = nextPosY.y;
                collisionFlags = filteredFlagsY;
                collisionFlags.insert(collisionFlags.end(), newFlagsY.begin(), newFlagsY.end());
                movedY = true;
            }

            // If both blocked, stop further movement steps
            if (!movedX && !movedY)
            {
                break;
            }
        }

        // Cleanup flags outside bounding box
        collisionFlags.erase(
            std::remove_if(
                collisionFlags.begin(),
                collisionFlags.end(),
                [&](const CollisionFlag &flag)
                {
                    return flag.x < roundToInt(entity->position.x) ||
                           flag.x >= roundToInt(entity->position.x) + entity->getWidth() ||
                           flag.y < roundToInt(entity->position.y) ||
                           flag.y >= roundToInt(entity->position.y) + entity->getHeight();
                }),
            collisionFlags.end());

        //  Debug::Log("Player position: " + std::to_string(entity->position.x) + ", " + std::to_string(entity->position.y));
    }

    // Check if a pixel at (localX, localY) in the sprite collides with a solid map pixel at (mapX, mapY)
    bool pixelPerfectCollisionAt(
        std::shared_ptr<Sprite> sprite,
        int localX, int localY,
        const Map *map,
        int mapX, int mapY)
    {
        // Check sprite pixel solidity
        if (sprite->isSolid(localX, localY) == 0)
            return false;

        // Check map pixel solidity
        if (!map->isSolid(mapX, mapY))
            return false;

        return true; // collision detected
    }

    bool isBoxCollidingWithMap(
        const Map *map,
        int x, int y,
        int width, int height,
        std::shared_ptr<Sprite> sprite,
        std::vector<CollisionFlag> &outFlags)
    {
        bool collisionFound = false;
        outFlags.clear();

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

                    // Check sprite pixel alpha (you need a function for this)
                    if (sprite->isSolid(localX, localY) > 0)
                    {
                        // Solid pixel collision: immediate collision
                        collisionFound = true;
                        // Optionally break here to optimize, or continue to collect flags
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
};