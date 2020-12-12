#pragma once

#include "algebraic.hpp"
#include <FlatAlg.hpp>

namespace generelle {

    /*
     * Box - axis-aligned and centered at origin. The constructor takes half edge lengths
     */

    class Box : public InnerGeometricExpression {
        falg::Vec3 span;

    public:

        Box(const falg::Vec3& span);

        virtual float signedDist(const falg::Vec3& pos) const;
    };



    /*
     * Cylinder - aligned with x-axis, centered at origo
     */

    class Cylinder : public InnerGeometricExpression {
        float radius, half_length;

    public:

        Cylinder(float radius, float length);

        virtual float signedDist(const falg::Vec3& pos) const;
    };



    /*
     * Sphere - centered at origin
     */

    class Sphere : public InnerGeometricExpression {
        float radius;
    public:

        Sphere(float radius);

        virtual float signedDist(const falg::Vec3& pos) const;
        virtual falg::Vec3 normal(const falg::Vec3& pos) const;
    };

    GE makeBox(const falg::Vec3& span);
    GE makeCylinder(float radius, float length);
    GE makeSphere(float radius);

};
