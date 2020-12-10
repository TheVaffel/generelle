#pragma once

#include "algebraic.hpp"
#include <FlatAlg.hpp>

namespace generelle {

    /*
     * Box - axis-aligned and centered at origin
     */
    
    class Box : public InnerGeometricExpression {
        falg::Vec3 span;

    public:

        Box(const falg::Vec3& span);

        virtual float signedDist(const falg::Vec3& pos) const;
    };

    GE makeBox(const falg::Vec3& span);
};
