#pragma once
#include "IBehavior.h"
#include <unordered_map>
#include <typeindex>
#include <memory>

template <typename T>
class BehaviorManager {
private:
    std::unordered_map<std::type_index, std::shared_ptr<IBehavior<T>>> behaviors;

public:
    std::shared_ptr<T> owner;

    BehaviorManager(std::shared_ptr<T> ownerObject) : owner(ownerObject) {}

    template <typename B>
    void addBehavior(std::shared_ptr<B> behavior) {
        static_assert(std::is_base_of<IBehavior<T>, B>::value, "Behavior must inherit from IBehavior<T>");
        behavior->attach(owner);
        behaviors[typeid(B)] = behavior;
    }

    template <typename B>
    std::shared_ptr<B> getBehavior() {
        auto it = behaviors.find(typeid(B));
        if (it != behaviors.end())
            return std::static_pointer_cast<B>(it->second);
        return nullptr;
    }
};
