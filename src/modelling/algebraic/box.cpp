#include "box.hpp"

namespace generelle {
    Box::Box(const falg::Vec3& span) : span(span) { }

    float Box::signedDist(const falg::Vec3& pos) const {
        float adx = std::abs(pos.x()) - std::abs(span.x());
        float ady = std::abs(pos.y()) - std::abs(span.y());
        float adz = std::abs(pos.z()) - std::abs(span.z());
        
        float dx = std::max(adx, 0.0f);
        float dy = std::max(ady, 0.0f);
        float dz = std::max(adz, 0.0f);

        float dd = sqrt(dx * dx + dy * dy + dz * dz);

        float idx = std::min(adx, 0.0f);
        float idy = std::min(ady, 0.0f);
        float idz = std::min(adz, 0.0f);

        float di = std::max(std::max(idx, idy), idz);

        return dd < 0 ? di : dd;
    }

    GE makeBox(const falg::Vec3& pos) {
        return GeometricExpression(new Box(pos));
    }
};
