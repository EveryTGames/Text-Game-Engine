#pragma once
#include "../Behaviores/IBehavior.h"
#include "../Entity/Entity.h"
#include "CollisionSystem.h"
#include <vector>

class CollisionBehavior : public IBehavior<Entity>
{
private:
    std::shared_ptr<Entity> owner;
    std::vector<CollisionNS::CollisionFlag> collisionFlags;

public:
    void attach(std::shared_ptr<Entity> owner) override { this->owner = owner; }

    // Applies movement while resolving collisions against the map using the sprite.
    // Returns the new position after collision resolution.
    Vec2 moveWithCollision(const Vec2 &currentPos, const Vec2 &moveAmount, const Map *map, std::shared_ptr<Sprite> sprite);

    // Optional: expose internal flags for debugging or other systems
    const std::vector<CollisionNS::CollisionFlag>& getCollisionFlags() const { return collisionFlags; }

private:
    void addFlagsNoDuplicates(const std::vector<CollisionNS::CollisionFlag> &newFlags);
};