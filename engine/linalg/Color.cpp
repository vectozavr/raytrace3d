//
// Created by Ivan Ilin on 13/09/2023.
//


#include <cmath>
#include <stdexcept>

#include <linalg/Color.h>
#include <Consts.h>

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    _arr_point[0] = r;
    _arr_point[1] = g;
    _arr_point[2] = b;
    _arr_point[3] = a;
}

Color::Color(const Color &color) {
    _arr_point[0] = color.r();
    _arr_point[1] = color.g();
    _arr_point[2] = color.b();
    _arr_point[3] = color.a();
}


// Operations with Vec4D
Color Color::operator+(const Color &color) const {
    return Color((uint8_t)std::min((double)r() + (double)color.r(), 255.0),
                 (uint8_t)std::min((double)g() + (double)color.g(), 255.0),
                 (uint8_t)std::min((double)b() + (double)color.b(), 255.0),
                 (uint8_t)std::min((double)a() + (double)color.a(), 255.0));
}

Color Color::operator-(const Color &color) const {
    return Color(r() - color.r(), g() - color.g(), b() - color.b(), a() - color.a());
}

Color Color::operator*(double number) const {
    return Color((uint8_t)(std::max(std::min((double)r() * number, 255.0), 0.0)),
                 (uint8_t)(std::max(std::min((double)g() * number, 255.0), 0.0)),
                 (uint8_t)(std::max(std::min((double)b() * number, 255.0), 0.0)),
                 a());
}

Color Color::operator/(double number) const {
    if (std::abs(number) > Consts::EPS) {
        return *this * (1.0 / number);
    } else {
        throw std::domain_error{"Color::operator/(double number): division by zero"};
    }
}

Color Color::normalized() const {
    double vecAbs = sqrt(r()*r() + g()*g() + b()*b() + a()*a());
    if (vecAbs > Consts::EPS) {
        return *this / vecAbs;
    } else {
        return Color(1);
    }
}

bool Color::operator==(const Color &color) const {
    return Vec4D(r(), g(), b(), a()) == Vec4D(color.r(), color.g(), color.b(), color.a());
}

Color Color::operator*(const Color &color) const {
    return Color((uint8_t)((double)(r()*color.r())/255.0),
                 (uint8_t)((double)(g()*color.g())/255.0),
                 (uint8_t)((double)(b()*color.b())/255.0),
                 (uint8_t)((double)(a()*color.a())/255.0));
}
