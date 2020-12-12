#include "cylinder.hpp"

namespace generelle {
    Cylinder::Cylinder(float radius, float length) : radius(radius), half_length(length / 2) { }

    float Cylinder::signedDist(const falg::Vec3& pos) const {
        float dr = falg::Vec2(pos.y(), pos.z()).norm() - radius;

        float dx = std::abs(pos.x()) - half_length;

        return std::min(std::max(dx, dr), sqrtf(dx * dx + dr * dr));
    }

    GeometricExpression makeCylinder(float radius, float length) {
        return GeometricExpression(new Cylinder(radius, length));
    }
};
