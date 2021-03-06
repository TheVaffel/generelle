#include "algebraic.hpp"

#include <vector>

#include <FlatAlg.hpp>
#include <HGraf.hpp>

namespace generelle {

    namespace MeshConstructor {


        struct ConstructMeshSetup {
            int numRectify = 0;
            bool includeSimplify = false;
        };

        void deduplicateMapPoints(const std::vector<falg::Vec3>& vertices, std::vector<int>& indexMap, float closest_distance);
        hg::NormalMesh constructMesh(const GeometricExpression& ge,
                                     float target_resolution = 0.1f,
                                     float start_span = 1e8,
                                     const falg::Vec3& mid = falg::Vec3(0.0f, 0.0f, 0.0f),
                                     const ConstructMeshSetup& meshSetup = ConstructMeshSetup());
    };

    typedef hg::NormalMesh Mesh;
};
