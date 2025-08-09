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
    std::ostringstream oss;
    oss << "\x1b[H"; // Move cursor to top

    auto &buf = buffers[currentBuffer];
    for (int y = 0; y < height; y += 2)
    {
        for (int x = 0; x < width; ++x)
        {
            RGBA top = buf[y][x];
            RGBA bottom = (y + 1 < height) ? buf[y + 1][x] : RGBA{0, 0, 0, 0};

            oss << "\x1b[38;2;" << (int)top.r << ";" << (int)top.g << ";" << (int)top.b << "m";
            oss << "\x1b[48;2;" << (int)bottom.r << ";" << (int)bottom.g << ";" << (int)bottom.b << "m";
            oss << "▀";
        }
        oss << "\x1b[0m\n";
    }
    frameStrings[currentBuffer] = oss.str();
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
    std::cout << frameStrings[1 - currentBuffer] << std::flush;
}