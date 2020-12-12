#pragma once

#include "algebraic.hpp"

namespace generelle {
    class Sphere : public InnerGeometricExpression {
        float radius;
    public:

        Sphere(float radius);

        virtual float signedDist(const falg::Vec3& pos) const;
        virtual falg::Vec3 normal(const falg::Vec3& pos) const;
    };

    GE makeSphere(float radius);
};
