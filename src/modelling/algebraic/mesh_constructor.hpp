#include "algebraic.hpp"

#include <vector>

#include <FlatAlg.hpp>

namespace generelle {
    struct Mesh {
        std::vector<falg::Vec3> positions, normals;
        std::vector<uint32_t> indices;

        int getNumIndices() const;
        int getNumVertices() const;
    };

    namespace MeshConstructor {
        Mesh constructMesh(const GeometricExpression& ge,
                           float target_resolution = 0.1f,
                           float start_span = 1e8,
                           const falg::Vec3& mid = falg::Vec3(0.0f, 0.0f, 0.0f));
    };
};
