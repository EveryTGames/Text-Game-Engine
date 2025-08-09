
enum class PixelType {
    Empty,      // alpha == 0 → fully transparent
    Trigger,    // alpha == 50 → special trigger zone
    Solid,      // everything else → collision
};

inline PixelType classifyPixelAlpha(int alpha) {
    if (alpha == 0) return PixelType::Empty;
    if (alpha == 50) return PixelType::Trigger;
    return PixelType::Solid;
}