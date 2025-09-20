#include "World.h"
#include "../Entity/Entity.h"

void World::update(float deltaTime)
{
    for (auto &entity : entities)
        entity->update(deltaTime);

    //  // Example: follow the player (first entity)
    // if (!entities.empty()) {
    //     camera.follow(entities[0]->position);
    // }
}

// Define static pointer
World *World::instance = nullptr;

// Return the singleton instance reference
World &World::GetInstance()
{
    if (!instance)
        throw std::runtime_error("World instance not set!");
    return *instance;
}

void World::addEntity(const std::shared_ptr<Entity> &entity)
{
    entity->world = this;
    entities.push_back(entity);
}
// Set the singleton instance pointer
void World::SetInstance(World *world)
{
    instance = world;
}

WorldRenderData World::collectRenderData() const
{
    WorldRenderData data;
    data.mapRender = map.getRenderData(camera);

    for (const auto &entity : entities)
    {
        RenderResult entityData = entity->getRenderData(camera);
        if (entityData.visible)
            data.entityRenders.push_back(entityData);
    }

    return data;
}
