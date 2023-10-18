//
// Created by Ivan Ilin on 07/10/2023.
//
#include <cmath>

#include "Sphere.h"

Sphere::Sphere(double radius, const Vec3D &position, const ObjectNameTag& nameTag, const Color& color)
: Object(nameTag), _radius(radius), _color(color) {
    translateToPoint(position);
}

Object::IntersectionInformation Sphere::intersect(const Vec3D &from, const Vec3D &to) const {
    double t = std::numeric_limits<double>::infinity();
    Vec3D norm{};

    Matrix4x4 toModelMatrix = invModel();

    Vec3D from_M = toModelMatrix*(from-position());
    Vec3D to_M = toModelMatrix*(to-position());

    // This should be done in the coordinate system of the object
    Vec3D d = to_M-from_M;
    Vec3D e_c = from_M;

    double d_ec = d.dot(e_c);
    double D = std::pow(d_ec, 2) - d.sqrAbs()*(e_c.sqrAbs()-_radius*_radius);
    if(D >= 0) {
        t = (-d_ec - std::sqrt(D))/d.dot(d);
        norm = (from_M + d*t)/_radius;
    }

    Vec3D point_M = from_M + d*t;

    Matrix4x4 fromModelMatrix = model();

    Vec3D point = fromModelMatrix*point_M + position();
    double distance = (point - from).abs();
    norm = (fromModelMatrix*norm).normalized();

    IntersectionInformation res = Object::IntersectionInformation{point,
                                           norm,
                                           distance,
                                           name(),
                                           std::make_shared<Object>(*this),
                                           (t > 0) && (std::abs(t) < std::numeric_limits<double>::infinity()),
                                           _color};

    return res;
}
