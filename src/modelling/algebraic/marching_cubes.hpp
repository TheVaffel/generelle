#include "algebraic.hpp"

#include <HGraf.hpp>

namespace generelle {
    namespace MarchingCubes {
	void marchingCubes(const GeometricExpression& ge,
                           std::vector<falg::Vec3>& vertices,
                           float target_span, float span, const falg::Vec3& mid);
    };
};
