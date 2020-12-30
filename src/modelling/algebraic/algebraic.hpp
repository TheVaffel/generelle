#pragma once

#include "declarations.hpp"

#include <FlatAlg.hpp>

#include <memory>

namespace generelle {

    /*
     * InnerGeometricExpression - A base class for
     */

    class InnerGeometricExpression {
    public:
        virtual float signedDist(const falg::Vec3& pos) const = 0;
        virtual falg::Vec3 normal(const falg::Vec3& pos) const;

        virtual ~InnerGeometricExpression() = 0;

        friend class GeometricExpression;
    };


    /*
     * GeometricExpression - base class for a node in the geometry algebra expression tree
     */

    class GeometricExpression {
        std::shared_ptr<InnerGeometricExpression> ige;
    public:
        GeometricExpression(InnerGeometricExpression* ig);
        GeometricExpression(const std::shared_ptr<InnerGeometricExpression>& ig);

        float signedDist(const falg::Vec3& pos) const;
        falg::Vec3 normal(const falg::Vec3& pos) const;

        GeometricExpression pad(float padding) const;
        GeometricExpression inverse() const;

        GeometricExpression add(const GeometricExpression& ge) const;
        GeometricExpression subtract(const GeometricExpression& ge) const;
        GeometricExpression smoothAdd(const GeometricExpression& ge, float k = 0.5f) const;
        GeometricExpression intersect(const GeometricExpression& ge) const;

        GeometricExpression translate(const falg::Vec3& d) const;
        GeometricExpression scale(const falg::Vec3& scale) const;
        GeometricExpression scale(float scale) const;
    };
};
