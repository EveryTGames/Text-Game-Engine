#include <windows.h>
#include <iostream>
#include <thread>
#include "Engine/Engine.h"
#include "Sprite/Sprite.h"
#include "Utils/Utils.h"
#include "PlayerMovement.h"

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

void setFullscreen()
{
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_MAXIMIZE);
}

void fixConsoleSize()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    SHORT width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SHORT height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    SMALL_RECT win = {0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1)};
    SetConsoleWindowInfo(hOut, TRUE, &win);

    COORD size = {width, height};
    SetConsoleScreenBufferSize(hOut, size);
}

void hideCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}
void getConsoleSize(int &width, int &height)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

/// TODO: add the rest of the Engine functnailities
/// TODO: add the map world things
/// TODO: map horizontal scrolling
/// TODO: Add error handling for sprite loading
/// TODO: Add sprite bounds checking
/// TODO: Add sprite animation
/// TODO: Add sprite rotation
int main()
{
    // ==== Console setup ====
    setFullscreen();
    fixConsoleSize();
    SetConsoleOutputCP(CP_UTF8);
    enableANSI();
    hideCursor();
    // ==== Debug setup ====
    Debug::Init();
    Debug::Log("Console setup complete");
    int width, height;
    getConsoleSize(width, height);
    height = height * 2 - 2; // Adjust for double-height rendering

    // ==== Engine + Game World ====
    Engine engine(width, height);

    auto world = std::make_unique<World>();
    // Load map
    Map gameMap("level1.png");
    world->setMap(gameMap);

    World::SetInstance(world.get()); // Set self-reference for scripts

    world->camera.setSize(width, height);

    // Load player
    auto playerSprite = std::make_shared<Sprite>("player.png", 5, 5);
    auto player = std::make_shared<Entity>(Vec2{3, 5}, playerSprite);
    // Add a movement script
    player->addScript<PlayerMovement>();
    world->addEntity(player);

    // // Camera follows player
    // world.camera.followTarget(player);

    // ==== Game loop ====
    const int targetFPS = 40;
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