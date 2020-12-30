#include "transformations.hpp"

namespace generelle {

    /*
     * GTranslate member functions
     */

    GTranslate::GTranslate(const IGE& s1, const falg::Vec3& d) : s1(s1), translation(d) { }

    float GTranslate::signedDist(const falg::Vec3& pos) const {
        return this->s1->signedDist(pos - this->translation);
    }

    /*
     * GNonUniformScale member functions
     */

    GNonUniformScale::GNonUniformScale(const IGE& s1, const falg::Vec3& scale)
        : s1(s1),
          inv_scale(falg::Vec3(1.0f / scale.x(), 1.0f / scale.y(), 1.0f / scale.z())), scale_norm(scale.norm()) { }

    float GNonUniformScale::signedDist(const falg::Vec3& pos) const {
        falg::Vec3 npos = pos * this->inv_scale;
        float back_scale = sqrtf(pos.sqNorm() / npos.sqNorm());
        return back_scale * this->s1->signedDist(pos * this->inv_scale);
    }


    /*
     * GUniformScale member functions
     */

    GUniformScale::GUniformScale(const IGE& s1, float scale) : s1(s1), inv_scale(1.0f / scale), scale(scale) { }

    float GUniformScale::signedDist(const falg::Vec3& pos) const {
        return this->scale * this->s1->signedDist(this->inv_scale * pos);
    }
};
