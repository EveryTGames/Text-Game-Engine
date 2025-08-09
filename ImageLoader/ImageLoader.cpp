#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Utils/stb_image.h"

bool ImageLoader::loadImage(
    const std::string& path,
    std::vector<std::vector<RGBA>>& pixels,
    int& width,
    int& height,
    int maxW,
    int maxH
) {
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4); // Force RGBA

    if (!data) {
        std::cerr << "Failed to load image: " << path << "\n";
        return false;
    }

    // Optional resize (nearest-neighbor)
    if ((maxW > 0 && width > maxW) || (maxH > 0 && height > maxH)) {
        float scaleX = (maxW > 0) ? (float)maxW / width : 1.0f;
        float scaleY = (maxH > 0) ? (float)maxH / height : 1.0f;
        float scale = std::min(scaleX, scaleY);

        int newW = (int)(width * scale);
        int newH = (int)(height * scale);
        std::vector<unsigned char> resized(newW * newH * 4);

        for (int y = 0; y < newH; ++y) {
            for (int x = 0; x < newW; ++x) {
                int srcX = (int)(x / scale);
                int srcY = (int)(y / scale);
                for (int c = 0; c < 4; ++c) {
                    resized[(y * newW + x) * 4 + c] = data[(srcY * width + srcX) * 4 + c];
                }
            }
        }

        stbi_image_free(data);
        data = resized.data(); // ❌ careful — this is a pointer into a vector that will go out of scope

        // Instead, directly write into `pixels` here:
        width = newW;
        height = newH;
        pixels.assign(height, std::vector<RGBA>(width));
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                pixels[y][x] = {
                    resized[(y * width + x) * 4 + 0],
                    resized[(y * width + x) * 4 + 1],
                    resized[(y * width + x) * 4 + 2],
                    resized[(y * width + x) * 4 + 3]
                };
        return true;
    }

    // No resize
    pixels.assign(height, std::vector<RGBA>(width));
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            pixels[y][x] = {
                data[(y * width + x) * 4 + 0],
                data[(y * width + x) * 4 + 1],
                data[(y * width + x) * 4 + 2],
                data[(y * width + x) * 4 + 3]
            };

    stbi_image_free(data);
    return true;
}
