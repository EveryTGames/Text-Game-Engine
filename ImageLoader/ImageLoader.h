#pragma once
#include <string>
#include <vector>
#include <iostream>

struct RGBA {
    unsigned char r, g, b, a; // 0 <= a <= 255
};

class ImageLoader {
public:
    static bool loadImage(
        const std::string& path,
        std::vector<std::vector<RGBA>>& pixels,
        int& width,
        int& height,
        int maxW = -1,
        int maxH = -1
    );
};