#include "sphere.hpp"

namespace generelle {
    Sphere::Sphere(float radius) : radius(radius) { }

    float Sphere::signedDist(const falg::Vec3& pos) const {
        return pos.norm() - this->radius;
    }

    falg::Vec3 Sphere::normal(const falg::Vec3& pos) const {
        return pos.normalized();
    }

    GeometricExpression makeSphere(float radius) {
        return GeometricExpression(new Sphere(radius));
    }
};
