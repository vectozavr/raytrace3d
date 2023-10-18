//
// Created by Ivan Ilin on 13/09/2023.
//

#ifndef RAYTRACE3D_COLOR_H
#define RAYTRACE3D_COLOR_H

#include <array>

class Color final {
private:
    std::array<uint8_t, 4> _arr_point{};

public:
    Color(const Color &color);

    explicit Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255);

    Color &operator=(const Color &color) = default;

    [[nodiscard]] uint8_t r() const { return _arr_point[0]; }
    [[nodiscard]] uint8_t g() const { return _arr_point[1]; }
    [[nodiscard]] uint8_t b() const { return _arr_point[2]; }
    [[nodiscard]] uint8_t a() const { return _arr_point[3]; }

    // Operations with colors
    [[nodiscard]] Color operator+(const Color &color) const;
    [[nodiscard]] Color operator-(const Color &color) const;
    [[nodiscard]] bool operator==(const Color &color) const;
    [[nodiscard]] Color operator*(const Color &color) const;

    // Operations with numbers
    [[nodiscard]] Color operator*(double number) const;
    [[nodiscard]] Color operator/(double number) const;

    // Other useful methods
    [[nodiscard]] Color normalized() const; // Returns normalized vector without changing
};


#endif //RAYTRACE3D_COLOR_H
