#include "algebraic.hpp"
#include "operations.hpp"
#include "transformations.hpp"

#include <algorithm>

namespace generelle {

    /*
     * InnerGeometricExpression member functions
     */

    falg::Vec3 InnerGeometricExpression::normal(const falg::Vec3& pos) const {
        float epsilon = 1e-5;

        falg::Vec3 v;
        for(int i = 0; i < 3; i++) {
            falg::Vec3 diff(0.0f, 0.0f, 0.0f);
            diff[i] = epsilon;
            v[i] = this->signedDist(pos + diff) - this->signedDist(pos - diff);
        }
        return v.normalized();
    }

    InnerGeometricExpression::~InnerGeometricExpression() { }


    /*
     * GeometricExpression member functions
     *  - Operations
     */

    GeometricExpression::GeometricExpression(InnerGeometricExpression* ig) {
        this->ige = std::shared_ptr<InnerGeometricExpression>(ig);
    }

    GeometricExpression::GeometricExpression(const std::shared_ptr<InnerGeometricExpression>& ig) : ige(ig) { }

    float GeometricExpression::signedDist(const falg::Vec3& pos) const {
        return this->ige->signedDist(pos);
    }

    falg::Vec3 GeometricExpression::normal(const falg::Vec3& pos) const {
        return this->ige->normal(pos);
    }

    GeometricExpression GeometricExpression::add(const GeometricExpression& ge) const {
        IGE ige(new GAdd(this->ige, ge.ige));

        return GeometricExpression(ige);
    }

    GE GeometricExpression::smoothAdd(const GE& ge, float k) const {
        IGE ige(new GSmoothAdd(this->ige, ge.ige, k));

        return GeometricExpression(ige);
    }

    GE GeometricExpression::pad(float padding) const {
        IGE ige(new GPad(this->ige, padding));
        return GeometricExpression(ige);
    }

    GE GeometricExpression::intersect(const GeometricExpression& ge) const {
        IGE ige(new GIntersect(this->ige, ge.ige));
        return GeometricExpression(ige);
    }

    GE GeometricExpression::inverse() const {
        IGE ige(new GInverse(this->ige));
        return GeometricExpression(ige);
    }

    GE GeometricExpression::subtract(const GeometricExpression& ge) const {
        IGE ige(new GIntersect(this->ige, IGE(new GInverse(ge.ige))));
        return GeometricExpression(ige);
    }


    /*
     * - Transformations
     */

    GE GeometricExpression::translate(const falg::Vec3& d) const {
        IGE ige(new GTranslate(this->ige, d));
        return GeometricExpression(ige);
    }

    GE GeometricExpression::scale(const falg::Vec3& scale) const {
        IGE ige(new GNonUniformScale(this->ige, scale));
        return GeometricExpression(ige);
    }

    GE GeometricExpression::scale(float scale) const {
        IGE ige(new GUniformScale(this->ige, scale));
        return GeometricExpression(ige);
    }

};
