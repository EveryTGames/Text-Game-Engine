#include "Engine.h"
#include <iostream>
#include <windows.h>
#include <../ImageLoader/ImageLoader.h>

Engine::Engine(int w, int h) : width(w), height(h)
{
    for (int i = 0; i < 2; ++i)
        buffers[i].resize(height, std::vector<RGBA>(width, {0, 0, 0, 0}));
}

void Engine::clearBuffer()
{
    auto &buf = buffers[currentBuffer];
    for (int y = 0; y < height; ++y)
        std::fill(buf[y].begin(), buf[y].end(), RGBA{0, 0, 0, 0});
}

void Engine::renderPixel(int x, int y, const RGBA &color)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        buffers[currentBuffer][y][x] = color;
}

void Engine::prepareFrameString()
{
    // Reserve space for full frame string (avoid reallocations)
    std::string &frame = frameStrings[currentBuffer];
    frame.clear();
    frame.reserve(width * height * estimatedCharsPerFrame);

    // Move cursor to top
    frame.append("\x1b[H");

    auto &buf = buffers[currentBuffer];

    // Track current colors to avoid redundant ANSI codes
    int lastFgR = -1, lastFgG = -1, lastFgB = -1;
    int lastBgR = -1, lastBgG = -1, lastBgB = -1;

    for (int y = 0; y < height; y += 2)
    {
        for (int x = 0; x < width; ++x)
        {
            const RGBA &top = buf[y][x];
            const RGBA &bottom = (y + 1 < height) ? buf[y + 1][x] : RGBA{0, 0, 0, 0};

            // Foreground (top half)
            if (top.r != lastFgR || top.g != lastFgG || top.b != lastFgB)
            {
                frame.append("\x1b[38;2;");
                frame.append(std::to_string(top.r));
                frame.push_back(';');
                frame.append(std::to_string(top.g));
                frame.push_back(';');
                frame.append(std::to_string(top.b));
                frame.push_back('m');
                lastFgR = top.r;
                lastFgG = top.g;
                lastFgB = top.b;
            }

            // Background (bottom half)
            if (bottom.r != lastBgR || bottom.g != lastBgG || bottom.b != lastBgB)
            {
                frame.append("\x1b[48;2;");
                frame.append(std::to_string(bottom.r));
                frame.push_back(';');
                frame.append(std::to_string(bottom.g));
                frame.push_back(';');
                frame.append(std::to_string(bottom.b));
                frame.push_back('m');
                lastBgR = bottom.r;
                lastBgG = bottom.g;
                lastBgB = bottom.b;
            }

            // Pixel block
            frame.append(u8"▀");
        }

        // Reset colors & new line
        frame.append("\x1b[0m\n");
        lastFgR = lastFgG = lastFgB = -1;
        lastBgR = lastBgG = lastBgB = -1;
    }
}

void Engine::swapBuffers()
{
    currentBuffer = 1 - currentBuffer;
}

void Engine::renderSprite(const RenderResult &data)
{
    if (!data.visible)
        return;

    auto &buf = buffers[currentBuffer];
    int height = data.visiblePixels.size();
    int width = data.visiblePixels[0].size();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const RGBA &px = data.visiblePixels[y][x];

            if (px.a == 0)
                continue; // Skip transparent pixels

            int bx = data.screenX + x;
            int by = data.screenY + y;

            if (bx >= 0 && bx < width && by >= 0 && by < height)
            {
                buf[by][bx] = px;
            }
        }
    }
}

void Engine::renderScene(const WorldRenderData &data)
{
    auto &buf = buffers[currentBuffer];

    // Draw map
    const RenderResult &m = data.mapRender;
    if (m.visible)
    {
        for (int y = 0; y < m.height; ++y)
            for (int x = 0; x < m.width; ++x)
                buf[m.screenY + y][m.screenX + x] = m.visiblePixels[y][x];
    }

    // Draw entities
    for (const auto &r : data.entityRenders)
    {
        if (!r.visible)
            continue;
        for (int y = 0; y < r.height; ++y)
        {

            for (int x = 0; x < r.width; ++x)
            {
                if(r.visiblePixels[y][x].a == 0)
                    continue; // Skip transparent pixels
                buf[r.screenY + y][r.screenX + x] = r.visiblePixels[y][x];
            }
        }
    }
}

void Engine::draw()
{
    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),
              frameStrings[1 - currentBuffer].c_str(),
              (DWORD)frameStrings[1 - currentBuffer].size(),
              &written,
              NULL);
}