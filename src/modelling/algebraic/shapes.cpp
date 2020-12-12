#include "shapes.hpp"

namespace generelle {

    /*
     * Box member functions
     */

    Box::Box(const falg::Vec3& span) : span(span) { }

    float Box::signedDist(const falg::Vec3& pos) const {
        float adx = std::abs(pos.x()) - std::abs(span.x());
        float ady = std::abs(pos.y()) - std::abs(span.y());
        float adz = std::abs(pos.z()) - std::abs(span.z());

        float dx = std::max(adx, 0.0f);
        float dy = std::max(ady, 0.0f);
        float dz = std::max(adz, 0.0f);

        float dd = sqrt(dx * dx + dy * dy + dz * dz);

        float idx = std::min(adx, 0.0f);
        float idy = std::min(ady, 0.0f);
        float idz = std::min(adz, 0.0f);

        float di = std::max(std::max(idx, idy), idz);

        return dd < 0 ? di : dd;
    }


    /*
     * Cylinder member functions
     */

    Cylinder::Cylinder(float radius, float length) : radius(radius), half_length(length / 2) { }

    float Cylinder::signedDist(const falg::Vec3& pos) const {
        float dr = falg::Vec2(pos.y(), pos.z()).norm() - radius;

        float dx = std::abs(pos.x()) - half_length;

        return std::min(std::max(dx, dr), sqrtf(dx * dx + dr * dr));
    }


    /*
     * Sphere member functions
     */

    Sphere::Sphere(float radius) : radius(radius) { }

    float Sphere::signedDist(const falg::Vec3& pos) const {
        return pos.norm() - this->radius;
    }

    falg::Vec3 Sphere::normal(const falg::Vec3& pos) const {
        return pos.normalized();
    }


    /*
     * Shape constructor functions
     */

    GE makeBox(const falg::Vec3& pos) {
        return GeometricExpression(new Box(pos));
    }

    GE makeCylinder(float radius, float length) {
        return GeometricExpression(new Cylinder(radius, length));
    }

    GE makeSphere(float radius) {
        return GeometricExpression(new Sphere(radius));
    }
};
