#pragma once

#include "algebraic.hpp"

namespace generelle {
    class Sphere : public InnerGeometricExpression {
        falg::Vec3 center;
        float radius;
    public:

        Sphere(const falg::Vec3& center, float radius);

        virtual float signedDist(const falg::Vec3& pos) const;
        virtual falg::Vec3 normal(const falg::Vec3& pos) const;
    };

    GE makeSphere(const falg::Vec3& center, float radius);
};
