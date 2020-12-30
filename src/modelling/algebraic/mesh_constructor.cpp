#include "mesh_constructor.hpp"
#include "marching_cube_info.hpp"

namespace generelle {

    /*
     * Mesh member functions
     */

    int Mesh::getNumIndices() const {
        return this->indices.size();
    }

    int Mesh::getNumVertices() const {
        return this->positions.size();
    }


    namespace MeshConstructor {
        const float sqrt3_ceil = sqrt(3) + 1.01f;
        const float target_span = 0.1f;
        const float epsilon = 1e-5;


        falg::Vec3 lerpVertex(float val0, float val1, const falg::Vec3& v0, const falg::Vec3& v1) {
            if (std::abs(val0 - val1) < epsilon) {
                return v0;
            }

            float mu = ( - val0) / (val1 - val0);
            return v0 + (v1 - v0) * mu;
        }

        // Assumes minBounds and maxBounds contain cubes
        void marchingCubes(const GeometricExpression& ge, Mesh* mesh, float target_span, float span, const falg::Vec3& mid) {
            float dist = ge.signedDist(mid);

            if (std::abs(dist) > span * sqrt3_ceil) {
                // This cube can't possibly intersect the geometry, return
                return;
            }

            if (span > target_span) {
                float nspan = span / 2;

                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        for (int k = 0; k < 2; k++) {
                            marchingCubes(ge, mesh, target_span, nspan,
                                          mid + falg::Vec3((2 * i - 1) * nspan,
                                                           (2 * j - 1) * nspan,
                                                           (2 * k - 1) * nspan));
                        }
                    }
                }
                return;
            }

            // The span of this cube is sufficiently small, create mesh here

            uint8_t corns = 0;
            float vals[8];
            for (int i = 0; i < 8; i++) {
                int fx = (i / 2) % 2;
                int fy = i / 4;
                int fz = i % 4 == 1 || i % 4 == 2;
                falg::Vec3 corn = mid + falg::Vec3((2 * fx - 1) * span,
                                                   (2 * fy - 1) * span,
                                                   (2 * fz - 1) * span);
                vals[i] = ge.signedDist(corn);
                corns |= (vals[i] > 0) << i;
            }

            // Allocate for all edges, use only those we need
            falg::Vec3 edgeVerts[12];

            for (int i = 0; i < 12; i++) {
                if (marching_cubes_info::edgeBitmasks[corns] & marching_cubes_info::edges[i].edgeFlag) {

                    int is[2] = { marching_cubes_info::edges[i].vert0,
                        marching_cubes_info::edges[i].vert1 };

                    falg::Vec3 vs[2] = { mid, mid };

                    for (int j = 0; j < 2; j++) {
                        vs[j][0] += (is[j] / 2) % 2 == 0 ? - span : span;
                        vs[j][1] += is[j] / 4 == 0 ? - span : span;
                        vs[j][2] += (is[j] % 4 == 0 || is[j] % 4 == 3) ? - span : span;
                    }
                    edgeVerts[i] = lerpVertex(vals[is[0]], vals[is[1]], vs[0], vs[1]);
                }
            }

            int vert_offset = mesh->getNumVertices();

            const int *tri = marching_cubes_info::triangleIndices[corns];

            for (int i = 0; tri[i] != -1; i += 3) {

                falg::Vec3 v0 = edgeVerts[tri[i + 0]];
                falg::Vec3 v1 = edgeVerts[tri[i + 1]];
                falg::Vec3 v2 = edgeVerts[tri[i + 2]];

                mesh->positions.push_back(v0);
                mesh->positions.push_back(v1);
                mesh->positions.push_back(v2);

                mesh->normals.push_back(ge.normal(v0));
                mesh->normals.push_back(ge.normal(v1));
                mesh->normals.push_back(ge.normal(v2));

                mesh->indices.push_back(vert_offset + i + 0);
                mesh->indices.push_back(vert_offset + i + 2);
                mesh->indices.push_back(vert_offset + i + 1);

            }

        }


        Mesh constructMesh(const GeometricExpression& ge, float target_resolution, float span, const falg::Vec3& mid) {
            Mesh mesh;

            marchingCubes(ge, &mesh, target_resolution / 2, span, mid);
            return mesh;
        }
    };
};
