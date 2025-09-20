#pragma once
#include "../Map/Map.h"
#include "../Camera/Camera.h"
#include "../World/World.h"
#include <vector>
#include <memory>
#include <algorithm>

struct WorldRenderData
{
    RenderResult mapRender;                  // The visible section of the map
    std::vector<RenderResult> entityRenders; // All visible entities
};

class Entity; // Forward declare to break circular include

class World
{
public:
    Camera camera;
    std::vector<std::shared_ptr<Entity>> entities;

    void update(float deltaTime);
    WorldRenderData collectRenderData() const;
    void setMap(const Map &newMap)
    {
        map = newMap; // store copy
    }

    // Access the singleton instance
    static World &GetInstance();
    // To set the singleton world instance once
    static void SetInstance(World *world);
    // Add an entity to the world
    void addEntity(const std::shared_ptr<Entity> &entity);
    // Remove entity by pointer
    void removeEntity(std::shared_ptr<Entity> entity)
    {
        entities.erase(
            std::remove(entities.begin(), entities.end(), entity),
            entities.end());
    }

    // Remove all entities of a certain type or condition
    template <typename Predicate>
    void removeEntitiesIf(Predicate pred)
    {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(), [&](const std::shared_ptr<Entity> &e)
                           { return pred(e); }),
            entities.end());
    }
    const Map &getMap() const { return map; }

private:
    Map map;
    static World *instance;
    // Self-reference for scripts
};
