// Script.h
#pragma once
#include <memory>

class Entity; // Forward declaration

class Script {
public:
    std::shared_ptr<Entity> entity; // The entity this script is attached to

    virtual ~Script() = default;

    virtual void onStart() {}     // Called when script is first added
    virtual void onUpdate(float dt) {} // Called every frame
    virtual void onDestroy() {}   // Called when entity or script is destroyed
};