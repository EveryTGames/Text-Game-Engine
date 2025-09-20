#include <windows.h>
#include <iostream>
#include <thread>
#include "Engine/Engine.h"
#include "Sprite/Sprite.h"
#include "Utils/Utils.h"
#include "PlayerMovement.h"
#include "../SpriteRotation/SpriteRotationBehavior.h"
#include "../Behaviores/BehaviorManager.h"

void enableANSI()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    if (!GetConsoleMode(hOut, &dwMode))
        return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

HWND waitForConsole()
{
    HWND hwnd = nullptr;
    HWND owner = nullptr;
    for (int i = 0; i < 200; ++i) // try longer if needed 
    {
           

        hwnd = GetConsoleWindow();
        if (hwnd != nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            
            owner = GetWindow(hwnd, GW_OWNER);         // for Windows 11
            if (hwnd != nullptr || (owner == nullptr)) // Windows 10 or 11
            {

                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return hwnd;
}
// ==== Make the console window fullscreen (window only, no borders) ====
void setFullscreenWindowOnly()
{
    HWND hwnd = waitForConsole();
    if (!hwnd)
    {

        return;
    }

    HWND target = hwnd;
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner != nullptr)
        target = owner;

    // Remove decorations
    LONG style = GetWindowLong(target, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    SetWindowLong(target, GWL_STYLE, style);

    // Get monitor dimensions
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromWindow(target, MONITOR_DEFAULTTOPRIMARY), &mi))
        return;

    SetWindowPos(target, HWND_TOP,
                 mi.rcMonitor.left,
                 mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOZORDER | SWP_FRAMECHANGED);

    DrawMenuBar(target);
}

// ==== Set a fixed console grid for proper engine rendering ====
void fixConsoleBufferSize(int cols, int rows)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    // Resize the buffer
    COORD bufferSize = {static_cast<SHORT>(cols), static_cast<SHORT>(rows)};
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // Resize the window rectangle
    SMALL_RECT windowSize = {0, 0, static_cast<SHORT>(cols - 1), static_cast<SHORT>(rows - 1)};
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}

// ==== hide cursor ====
void hideCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

// ==== get console grid size ====
void getConsoleSize(int &width, int &height)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

/// TODO: Add the behaviour for sprite animation
/// TODO: make the collistion system a behavoure and call it in the player movement normally, or even in the physics 
/// TODO: add the behaviour for physics or maybe not a behavoure since i willl need to call it every certain frames, or maybe a behavoiuor and make it controlled inside the main loop, but this will need to map al the entities that  has attached physics behaviour, depanding in the best practise 
/// TODO: Add ui system 
/// TODO: Add multy windows(cmds) system for displaying texts (with wrapping option or maximum characters in one line and these text stuff)
/// TODO: add the behaviour of platforms


int main()
{
    Sleep(50);

   
    //  ==== Console setup ====
    setFullscreenWindowOnly();
    SetConsoleOutputCP(CP_UTF8);
    enableANSI();
    hideCursor();
    // ==== Debug setup ====
    Debug::Init();
    Debug::Log("Console setup complete");
    int width, height;
    getConsoleSize(width, height);
    fixConsoleBufferSize(width, height);
    height = height * 2 - 2 ; // Adjust for double-height rendering

    // ==== Engine + Game World ====
    Engine engine(width, height);

    auto world = std::make_unique<World>();
    // Load map
    Map gameMap("level1.png");
    world->setMap(gameMap);

    World::SetInstance(world.get()); // Set self-reference for scripts

    world->camera.setSize(width, height);

    // Load player
    auto playerSprite = Sprite::create("player.png", 5, 5);

    
    // Attach the rotation behavior
    playerSprite->getBehaviorManager().addBehavior(std::make_shared<SpriteRotationBehavior>());

    auto player = Entity::create(Vec2{3, 5}, playerSprite);
    // Add a movement script
    
    player->addScript<PlayerMovement>();
    world->addEntity(player);

    // // Camera follows player
    // world.camera.followTarget(player);

    // ==== Game loop ====
    const int targetFPS = 60;

    const auto frameDelay = std::chrono::milliseconds(1000 / targetFPS);

    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (true)
    {

        auto frameStart = std::chrono::high_resolution_clock::now();

        // 1. Update world logic
        float deltaTime = 1.0f / targetFPS; // Fixed timestep
        world->update(deltaTime);

        // 2. Gather render data
        WorldRenderData renderData = world->collectRenderData();

        // 3. Clear buffer & draw
        engine.clearBuffer();
        engine.renderScene(renderData);
        engine.prepareFrameString();
        engine.swapBuffers();
        engine.draw();

        // 4. Frame timing
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        if (elapsed < frameDelay)
            std::this_thread::sleep_for(frameDelay - elapsed);

        frameCount++;

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - startTime;

        if (elapsedTime.count() >= 1.0f)
        {
            float fps = frameCount / elapsedTime.count();
            Debug::Log("FPS: " + std::to_string(fps));

            // Reset for the next second
            frameCount = 0;
            startTime = currentTime;
        }
    }

    return 0;
}