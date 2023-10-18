//
// Created by Иван Ильин on 13.01.2021.
//

#ifndef ENGINE_TRIANGLE_H
#define ENGINE_TRIANGLE_H

#include "linalg/Vec4D.h"
#include "linalg/Vec3D.h"
#include "linalg/Color.h"
#include "linalg/Matrix4x4.h"

class Triangle final {
private:
    Color _color;
    Vec4D _points[3];
    Vec3D _normal;

    void calculateNormal();
public:
    Triangle() = default;

    Triangle(const Triangle &triangle);

    Triangle(const Vec4D &p1, const Vec4D &p2, const Vec4D &p3, const Color& color = Color());

    Triangle &operator=(const Triangle &) = default;

    [[nodiscard]] const Vec4D& operator[](int i) const;

    [[nodiscard]] Vec3D position() const { return Vec3D(_points[0] + _points[1] + _points[2])/3; }

    [[nodiscard]] Vec3D norm() const;

    // Operations with Matrix4x4
    [[nodiscard]] Triangle operator*(const Matrix4x4 &matrix4X4) const;

    [[nodiscard]] bool isPointInside(const Vec3D &point) const;

    [[nodiscard]] Color color() const { return _color; }

    void setColor(const Color& newColor) { _color = newColor; }

    [[nodiscard]] double distance(const Vec3D &vec) const { return norm().dot(Vec3D(_points[0]) - vec); }
};


#endif //INC_3DZAVR_TRIANGLE_H
