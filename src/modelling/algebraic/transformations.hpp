#pragma once

#include "algebraic.hpp"

#include <FlatAlg.hpp>

namespace generelle {

    /*
     * GTranslate - translates / moves the model
     */

    class GTranslate : public InnerGeometricExpression {
        const IGE s1;
        const falg::Vec3 translation;
    public:
        GTranslate(const IGE& s1, const falg::Vec3& d);

        virtual float signedDist(const falg::Vec3& pos) const;
    };


    /*
     * GNonUniformScale - scales the model with separate scalar per axis
     */

    class GNonUniformScale : public InnerGeometricExpression {
        const IGE s1;
        falg::Vec3 inv_scale;
        float scale_norm;

    public:
        GNonUniformScale(const IGE& s1, const falg::Vec3& scale);

        virtual float signedDist(const falg::Vec3& pos) const;
    };


    /*
     * GUniformScale - scales the model uniformly (the same scale in each axis)
     */

    class GUniformScale : public InnerGeometricExpression {
        const IGE s1;
        float inv_scale, scale;
    public:
        GUniformScale(const IGE& s1, float scale);

        virtual float signedDist(const falg::Vec3& pos) const;
    };
};
