#include "sphere.hpp"

namespace generelle {
    Sphere::Sphere(const falg::Vec3& center, float radius) : radius(radius), center(center) { }

    float Sphere::signedDist(const falg::Vec3& pos) const {
        return (pos - this->center).norm() - this->radius;
    }

    falg::Vec3 Sphere::normal(const falg::Vec3& pos) const {
        return (pos - this->center).normalized();
    }

    GeometricExpression makeSphere(const falg::Vec3& center, float radius) {
        return GeometricExpression(new Sphere(center, radius));;
    }
};
