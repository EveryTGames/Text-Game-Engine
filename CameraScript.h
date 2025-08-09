#pragma once
#include "Script.h"
#include "Entity/Entity.h"
#include "../Utils/Debug.h"
#include "PlayerMovement.h"



CaneraScript camera; // self-reference for easier access

class CaneraScript : public Script
{
    public:
    PlayerMovement player; 
    void onStart() override
    {
      
      
      
        player = PlayerMovement::GetInstance(); // Get the player movement instance
        // Initialize velocity
        Debug::Log("camera script started");
    }
    void onUpdate(float dt) override
    {
     
       

    }
};
