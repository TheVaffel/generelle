#pragma once

#include "algebraic.hpp"

namespace generelle {

    /*
     * Cylinder, aligned with x-axis, centered at origo
     */

    class Cylinder : public InnerGeometricExpression {
        float radius, half_length;

    public:

        Cylinder(float radius, float length);

        virtual float signedDist(const falg::Vec3& pos) const;
    };

    GeometricExpression makeCylinder(float radius, float length);
};
