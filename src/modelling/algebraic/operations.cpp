#include "operations.hpp"

namespace generelle {

    /*
     * GAdd member functions
     */

    GAdd::GAdd(const IGE& s1,
               const IGE& s2) : s1(s1), s2(s2) { }

    float GAdd::signedDist(const falg::Vec3& pos) const {
        return std::min(this->s1->signedDist(pos), this->s2->signedDist(pos));
    }

    falg::Vec3 GAdd::normal(const falg::Vec3& pos) const {
        float c1 = this->s1->signedDist(pos);
        float c2 = this->s2->signedDist(pos);
        if (std::min(c1, c2) == c1) {
            return this->s1->normal(pos);
        } else {
            return this->s2->normal(pos);
        }
    }


    /*
     * GSmoothAdd member functions
     */

    GSmoothAdd::GSmoothAdd(const IGE& s1, const IGE& s2,
                           float k) : s1(s1), s2(s2), k(k) { }

    float GSmoothAdd::signedDist(const falg::Vec3& pos) const {
        float c1 = this->s1->signedDist(pos);
        float c2 = this->s2->signedDist(pos);

        return std::min(c1, c2) - std::pow(std::max(this->k - std::abs(c1 - c2), 0.f), 3) / (6 * this->k * this->k);
    }


    /*
     * GPad member functions
     */

    GPad::GPad(const IGE& s1, float r) : s1(s1), r(r) { }

    float GPad::signedDist(const falg::Vec3& pos) const {
        return this->s1->signedDist(pos) - r;
    }


    /*
     * GIntersect member functions
     */

    GIntersect::GIntersect(const IGE& s1, const IGE& s2) : s1(s1), s2(s2) { }

    float GIntersect::signedDist(const falg::Vec3& pos) const {
        return std::max(this->s1->signedDist(pos), this->s2->signedDist(pos));
    }


    /*
     * GInverse member functions
     */

    GInverse::GInverse(const IGE& s1) : s1(s1) { }

    float GInverse::signedDist(const falg::Vec3& pos) const {
        return - this->s1->signedDist(pos);
    }
};
