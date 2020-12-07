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

        GeometricExpression add(const GeometricExpression& ge) const;
        GeometricExpression smoothAdd(const GeometricExpression& ge, float k = 0.5f) const;
    };
    
};