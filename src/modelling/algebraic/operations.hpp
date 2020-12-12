#pragma once

#include "algebraic.hpp"

namespace generelle {

    typedef std::shared_ptr<InnerGeometricExpression> IGE;
    
    /*
     * GAdd - An expression tree node representing a shape addition
     */
    
    class GAdd : public InnerGeometricExpression {
        const IGE s1, s2;
    public:
        GAdd(const IGE& s1,
             const IGE& s2);
        
        virtual float signedDist(const falg::Vec3& pos) const;
        virtual falg::Vec3 normal(const falg::Vec3& pos) const;
    };


    /*
     * GSmoothAdd - like GAdd, but transitions between geometries are made differentiable
     */

    class GSmoothAdd : public InnerGeometricExpression {
        const IGE s1, s2;
        float k;
    public:
        GSmoothAdd(const IGE& s1,
                   const IGE& s2, float k);

        virtual float signedDist(const falg::Vec3& pos) const;
    };


    /*
     * GPad - Adds padding to the object with a given radius
     */

    class GPad : public InnerGeometricExpression {
        const IGE s1;
        float r;
    public:
        GPad(const IGE& s1, float r);

        virtual float signedDist(const falg::Vec3& pos) const;
    };
};
