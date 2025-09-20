#include <algorithm>

namespace RotationUtils {
    RGBA sampleBilinear(const std::vector<std::vector<RGBA>>& pixels, float x, float y) {
        int w = pixels[0].size();
        int h = pixels.size();

        int x0 = std::floor(x);
        int y0 = std::floor(y);
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        auto clamp = [](int val, int minV, int maxV) { return std::max(minV, std::min(maxV, val)); };

        x0 = clamp(x0, 0, w - 1);
        y0 = clamp(y0, 0, h - 1);
        x1 = clamp(x1, 0, w - 1);
        y1 = clamp(y1, 0, h - 1);

        float dx = x - x0;
        float dy = y - y0;

        auto lerp = [](RGBA a, RGBA b, float t) -> RGBA {
            return {
                static_cast<unsigned char>(a.r + (b.r - a.r) * t),
                static_cast<unsigned char>(a.g + (b.g - a.g) * t),
                static_cast<unsigned char>(a.b + (b.b - a.b) * t),
                static_cast<unsigned char>(a.a + (b.a - a.a) * t)
            };
        };

        RGBA top = lerp(pixels[y0][x0], pixels[y0][x1], dx);
        RGBA bottom = lerp(pixels[y1][x0], pixels[y1][x1], dx);
        return lerp(top, bottom, dy);
    }

    std::vector<std::vector<RGBA>> rotateSpritePixels(const std::vector<std::vector<RGBA>>& pixels, float angleDegrees) {
        int srcW = pixels[0].size();
        int srcH = pixels.size();
        int dstW = srcW;
        int dstH = srcH;

        float angleRad = angleDegrees * 3.14159265f / 180.0f;
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);

        int cx = srcW / 2;
        int cy = srcH / 2;

        std::vector<std::vector<RGBA>> rotated(dstH, std::vector<RGBA>(dstW, {0,0,0,0}));

        for (int y = 0; y < dstH; ++y) {
            for (int x = 0; x < dstW; ++x) {
                float relX = x - cx;
                float relY = y - cy;

                float srcX = relX * cosA + relY * sinA + cx;
                float srcY = -relX * sinA + relY * cosA + cy;

                if (srcX >= 0 && srcX < srcW && srcY >= 0 && srcY < srcH) {
                    rotated[y][x] = sampleBilinear(pixels, srcX, srcY);
                }
            }
        }

        return rotated;
    }
}
