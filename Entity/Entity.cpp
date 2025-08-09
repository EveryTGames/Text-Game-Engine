#include "Entity.h"
#include <iostream>
#include "../World/World.h"

void Entity::update(float deltaTime)
{

    updateScripts(deltaTime);
}

const Map *Entity::getMap() const
{
    return world ? &world->getMap() : nullptr;
}


RenderResult Entity::getRenderData(const Camera &cam) const
{
    int camX = cam.position.x;
    int camY = cam.position.y;
    int camW = cam.getWidth();
    int camH = cam.getHeight();

    // Check if entity is outside the camera view
    if (position.x + sprite->getWidth() < camX || position.x >= camX + camW || position.y + sprite->getHeight() < camY || position.y >= camY + camH)
        return RenderResult{.visible = false};

    // Calculate visible area
    int spriteStartX = std::max(0, camX - position.x);
    int spriteStartY = std::max(0, camY - position.y);
    int drawStartX = std::max(0, position.x - camX);
    int drawStartY = std::max(0, position.y - camY);
    int visibleWidth = std::min(sprite->getWidth() - spriteStartX, camW - drawStartX);
    int visibleHeight = std::min(sprite->getHeight() - spriteStartY, camH - drawStartY);

    return RenderResult{
        .screenX = drawStartX,
        .screenY = drawStartY,
        .width = visibleWidth,
        .height = visibleHeight,
        .visiblePixels = sprite->getPartialPixels(spriteStartX, spriteStartY, visibleWidth, visibleHeight),
        .visible = true};
}
