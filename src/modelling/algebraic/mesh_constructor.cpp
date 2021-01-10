#include "mesh_constructor.hpp"
#include "marching_cube_info.hpp"

#include <HGraf.hpp>

namespace generelle {

    namespace MeshConstructor {
        const float sqrt3_ceil = sqrt(3) + 1.01f;
        const float target_span = 0.1f;
        const float epsilon = 1e-5;
        const float vertex_epsilon = 1e-3;


        falg::Vec3 lerpVertex(float val0, float val1, const falg::Vec3& v0, const falg::Vec3& v1) {
            if (std::abs(val0 - val1) < epsilon) {
                return v0;
            }

            float mu = ( - val0) / (val1 - val0);

            if (mu < vertex_epsilon) {
                return v0;
            } else if (mu > 1 - vertex_epsilon) {
                return v1;
            }

            return v0 + (v1 - v0) * mu;
        }

        // Assumes minBounds and maxBounds contain cubes
        void marchingCubes(const GeometricExpression& ge,
                           std::vector<falg::Vec3>& vertices,
                           float target_span, float span, const falg::Vec3& mid) {
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
                            marchingCubes(ge, vertices, target_span, nspan,
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

            const int *tri = marching_cubes_info::triangleIndices[corns];

            for (int i = 0; tri[i] != -1; i += 3) {

                falg::Vec3 v0 = edgeVerts[tri[i + 0]];
                falg::Vec3 v1 = edgeVerts[tri[i + 1]];
                falg::Vec3 v2 = edgeVerts[tri[i + 2]];

                vertices.push_back(v0);
                vertices.push_back(v2);
                vertices.push_back(v1);
            }
        }

        // On return, for every vertex, indexMap gives the index of the first vertex in the list that is sufficiently close to this one
        void deduplicateMapPoints(const std::vector<falg::Vec3>& vertices, std::vector<int>& indexMap, float closest_distance) {

            indexMap = std::vector<int>(vertices.size(), -1);

            struct VertInd {
                falg::Vec3 vertex;
                unsigned int index;

                falg::Vec3 getPosition() const {
                    return vertex;
                }
            };

            std::vector<VertInd> vertInds;

            for (unsigned int i = 0; i < vertices.size(); i++) {
                vertInds.push_back(VertInd { vertices[i], i });
            }

            // Using a BVH to find close points fast
            std::shared_ptr<const hg::BVH<VertInd>> boundingVolumeHierarchy =
                hg::BVH<VertInd>::createBVH(vertInds.data(), vertInds.size());

            for (unsigned int i = 0; i < vertices.size(); i++) {
                std::vector<VertInd> foundVertInds;
                boundingVolumeHierarchy->getWithin(vertices[i], closest_distance, foundVertInds);
                for (unsigned int j = 0; j < foundVertInds.size(); j++) {
                    if (indexMap[foundVertInds[j].index] < 0) {
                        indexMap[foundVertInds[j].index] = i;
                    }
                }
            }
        }


        hg::NormalMesh constructMesh(const GeometricExpression& ge, float target_resolution, float span, const falg::Vec3& mid) {

            std::vector<falg::Vec3> temp_positions;
            marchingCubes(ge,
                          temp_positions,
                          target_resolution / 2, span, mid);


            std::vector<int> indMap;
            deduplicateMapPoints(temp_positions, indMap, 1e-6);

            // Yet another map, to map indices in the original vertex list
            // to indices in this reduced vertex list
            std::vector<int> newMap(indMap.size());

            hg::NormalMesh mesh;
            for (unsigned int i = 0; i < temp_positions.size(); i++) {
                if (indMap[i] == (int)i) {
                    mesh.indices.push_back(mesh.positions.size());
                    newMap[indMap[i]] = mesh.positions.size();

                    mesh.positions.push_back(temp_positions[i]);
                    mesh.normals.push_back(ge.normal(temp_positions[i]));
                } else {
                    mesh.indices.push_back(newMap[indMap[i]]);
                }
            }


            // Remove degenerate triangles (where two indices are the same)
            int num_removed = 0;
            for (unsigned int i = 0; i < mesh.indices.size() - 3 * num_removed; i++) {
                int tbase = (i / 3) * 3;
                int nextInd = ((i - tbase) + 1) % 3 + tbase;

                if (mesh.indices[i] == mesh.indices[nextInd]) {
                    // If degenerate, replace with last triangle in list that has not already been used as replacement
                    for (int j = 0; j < 3; j++) {
                        mesh.indices[tbase + j] = mesh.indices[mesh.indices.size() - 3 * (num_removed + 1) + j];
                    }
                    num_removed++;

                    // Re-evaluate this triangle
                    i = tbase;
                }
            }

            mesh.indices.resize(mesh.indices.size() - 3 * num_removed);

            hg::HalfEdgeMesh hem(mesh);

            return mesh;
        }
    };
};
