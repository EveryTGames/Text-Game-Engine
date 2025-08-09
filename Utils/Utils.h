#pragma once
#include <string>

struct Vec2 {
    int x;
    int y;

    Vec2() : x(0), y(0) {}
    Vec2(int x, int y) : x(x), y(y) {}

        // Overload the + operator
    Vec2 operator+(const Vec2& other) const {
        return Vec2{ x + other.x, y + other.y };
    }
    // Overload the - operator
    Vec2 operator-(const Vec2& other) const {
        return Vec2{ x - other.x, y - other.y };
    }

    // Overload += operator
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    // Overload the / operator for scalar division
    Vec2 operator/(float scalar) const {
        return Vec2{ static_cast<int>(x / scalar), static_cast<int>(y / scalar) };
    }

    // Overload the * operator for scalar multiplication
    Vec2 operator*(float scalar) const {
        return Vec2{ static_cast<int>(x * scalar), static_cast<int>(y * scalar) };
    }
};



struct Vec2float {
    float x;
    float y;

    Vec2float() : x(0), y(0) {}
    Vec2float(float x, float y) : x(x), y(y) {}

        // Overload the + operator
    Vec2float operator+(const Vec2float& other) const {
        return Vec2float{ x + other.x, y + other.y };
    }

    // Overload the - operator
    Vec2float operator-(const Vec2float& other) const {
        return Vec2float{ x - other.x, y - other.y };
    }
    // Overload += operator
    Vec2float& operator+=(const Vec2float& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    // Overload the * operator for scalar multiplication
    Vec2float operator*(float scalar) const {
        return Vec2float{ x * scalar, y * scalar };
    }

       // Conversion operator to Vec2
    operator Vec2() const {
        return Vec2(static_cast<int>(x), static_cast<int>(y));
    }
};


class Utils {
public:
 
};