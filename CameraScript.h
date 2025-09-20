#pragma once
#include "Script.h"
#include "Entity/Entity.h"
#include "../Utils/Debug.h"
#include "PlayerMovement.h"



CaneraScript camera; // self-reference for easier access

class CaneraScript : public Script
{
    public:
    std::shared_ptr<PlayerMovement> player; 
    void onStart() override
    {
      
      
      
        player = entity->getScript<PlayerMovement>(); // Get the player movement instance
        // Initialize velocity
        Debug::Log("camera script started");
    }
    void onUpdate(float dt) override
    {   
        
     
       

    }
};
