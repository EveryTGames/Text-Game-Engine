#pragma once
#include "Script.h"
#include <iostream>
#include "Entity/Entity.h"
#include "Map/Map.h"
#include "../Utils/Debug.h"
#include <cmath>
#include <vector>
#include <cmath> // for std::round
#include "../SpriteRotation/SpriteRotationBehavior.h"
#include "../Sprite/SpriteAnimationBehavior.h"
#include "../Collision/CollisionSystem.h"
#include "../Collision/CollisionBehavior.h"

// CollisionFlag moved to Collision/CollisionSystem.h

class PlayerMovement : public Script
{

public:
    PlayerMovement() {}

    World *world = nullptr;

    const Map *map;
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<SpriteRotationBehavior> rot;
    std::shared_ptr<SpriteAnimationBehavior> anim;
    std::shared_ptr<CollisionBehavior> collider;
    float angle;

    // Collision flags are now managed by the attached CollisionBehavior
    void onStart() override
    {
        world = &World::GetInstance();
        if (!entity)
        {
            Debug::Log(" PlayerMovement: entity is null");
            return;
        }

        angle = 90;

        map = entity->getMap();
        if (!map)
        {
            Debug::Log(" PlayerMovement: map is null");
        }

        sprite = entity->sprite;
        if (!sprite)
        {
            Debug::Log(" PlayerMovement: sprite is null");
            return;
        }

        rot = sprite->getBehavior<SpriteRotationBehavior>();
        if (!rot)
        {
            Debug::Log(" PlayerMovement: rotation behavior is null");
        }
        /// TODO you will make the sprite rotation in a seperated default script that be attached automaticliiy to any sprits created
        rot->setInitialCache(sprite->getPixels());

        // Setup animation behavior (auto-attach if missing)
        anim = sprite->getBehavior<SpriteAnimationBehavior>();
        if (!anim)
        {
            auto newAnim = std::make_shared<SpriteAnimationBehavior>();
            sprite->getBehaviorManager().addBehavior<SpriteAnimationBehavior>(newAnim);
            anim = newAnim;
        }
        // Load all frames from sheet tiles of 16x24 pixels:
        if (anim->addFramesFromSheet("assets/playerAnimation.png", 5, 5))
        {
            anim->setFrameDuration(0.3f);
            anim->setLooping(true);
            anim->play();
        }

        // Attach collision behavior to the entity (auto-create if missing)
        collider = entity->getBehavior<CollisionBehavior>();
        if (!collider)
        {
            auto col = std::make_shared<CollisionBehavior>();
            entity->getBehaviorManager().addBehavior<CollisionBehavior>(col);
            collider = col;
        }

        Debug::Log(" PlayerMovement script started");
    }
    inline int roundToInt(float val)
    {
        return static_cast<int>(std::round(val));
    }

    void onUpdate(float dt) override
    {

        world->camera.position = entity->position - Vec2{world->camera.getWidth() / 2, world->camera.getHeight() / 2}; // Center camera on player
                                                                                                                       //  Debug::Log("Camera position: " + std::to_string(world->camera.position.x) + ", " + std::to_string(world->camera.position.y));
                                                                                                                       //   Debug::Log("camera width and height: " + std::to_string(world->camera.getWidth() / 2) + ", " + std::to_string(world->camera.getHeight() / 2));
        // advance animation each frame
        if (anim)
            anim->update(dt);
        Vec2float inputVelocity{0.0f, 0.0f};

        if (GetAsyncKeyState('W') & 0x8000)
            inputVelocity.y = -1;
        if (GetAsyncKeyState('S') & 0x8000)
            inputVelocity.y = 1;
        if (GetAsyncKeyState('A') & 0x8000)
            inputVelocity.x = -1;
        if (GetAsyncKeyState('D') & 0x8000)
            inputVelocity.x = 1;
        if (GetAsyncKeyState('R') & 0x0001)
        {

            rot->rotate(270);
        }
        if (GetAsyncKeyState('T') & 0x0001)
        {

            rot->rotate(0);
        }
        if (GetAsyncKeyState('Y') & 0x0001)
        {
            /// TODO
            // i figured it out
            //  the problem is that when rotating it be rotating the new rotated spirit, so the angle be reset every rotation
            //  to fix it u can make a variable that hold the main spirit or u can just use the cach variable to store the main spirit when created at index 0
            // and instead of sednig the new spirit to the rotating function, we send the cached one at index zero

            rot->rotate(angle += 22.5);
        }

        if (inputVelocity.x != 0 && inputVelocity.y != 0)
        {
            float invLen = std::ceil(1.0f / std::sqrt(inputVelocity.x * inputVelocity.x + inputVelocity.y * inputVelocity.y));
            inputVelocity.x *= invLen;
            inputVelocity.y *= invLen;
            //
        }

        float speed = 1.0f;
        entity->velocity = inputVelocity * speed;

        // Compute desired movement and delegate collision resolution to the collider behavior if present
        Vec2 moveAmount = entity->velocity * std::ceil(dt * 30);
        if (collider)
        {
            entity->position = collider->moveWithCollision(entity->position, moveAmount, map, sprite);
        }
        else
        {
            // fallback: direct move (no collision)
            entity->position += moveAmount;
        }

        //  Debug::Log("Player position: " + std::to_string(entity->position.x) + ", " + std::to_string(entity->position.y));
    }

    // Collision is handled by the attached CollisionBehavior.
};