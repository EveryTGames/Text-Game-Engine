#include <memory>
#pragma once
#include "../Camera/Camera.h"
#include "../Map/Map.h"
#include "../ImageLoader/ImageLoader.h"
#include "../Utils/RenderResult.h"
#include "../Sprite/Sprite.h"
#include "../Utils/Utils.h"
#include "../World/World.h"
#include "../Script.h"
#include "../Behaviores/BehaviorAttachableBase.h"

class Entity : public BehaviorAttachableBase<Entity>
{
    
    DECLARE_BEHAVIOR_ATTACHABLE(Entity,
        (Vec2 pos, std::shared_ptr<Sprite> spr),
         {
            position = pos;
            sprite = spr;
            world = &World::GetInstance();
        }
    )

    public:
    Vec2 position;
    Vec2float velocity;
    std::shared_ptr<Sprite> sprite;
    World *world;



    void update(float dt);
    RenderResult getRenderData(const Camera &cam) const;

    // get width and height of the entity
    int getWidth() const { return sprite ? sprite->getWidth() : 0; }
    int getHeight() const { return sprite ? sprite->getHeight() : 0; }

    const Map *getMap() const; // Implementation will just call world->getMap()

    /// TODO: understand this better
    // Add script
    template <typename T, typename... Args>
    std::shared_ptr<T> addScript(Args &&...args)
    {
        static_assert(std::is_base_of<Script, T>::value, "T must inherit from Script");
        auto script = std::make_shared<T>(std::forward<Args>(args)...);
        script->entity = shared_from_this(); // correct shared_ptr
        scripts.push_back(script);
        script->onStart();
        return script;
    }

    template <typename T>
    std::shared_ptr<T> getScript()
    {
        static_assert(std::is_base_of<Script, T>::value, "T must inherit from Script");

        for (auto &script : scripts)
        {
            auto casted = std::dynamic_pointer_cast<T>(script);
            if (casted)
                return casted;
        }

        return nullptr; // Not found
    }

    void updateScripts(float dt)
    {
        for (auto &script : scripts)
        {
            script->onUpdate(dt);
        }
    }

    ~Entity()
    {
        for (auto &script : scripts)
        {
            script->onDestroy();
        }
    }

private:
    std::vector<std::shared_ptr<Script>> scripts;
};