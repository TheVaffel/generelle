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
                if (indexMap[i] > 0) {
                    // This vertex has already been mapped to another vertex. Don't merge neighbor points to this
                    continue;
                }

                std::vector<VertInd> foundVertInds;
                boundingVolumeHierarchy->getWithin(vertices[i], closest_distance, foundVertInds);
                for (unsigned int j = 0; j < foundVertInds.size(); j++) {
                    if (indexMap[foundVertInds[j].index] < 0) {
                        indexMap[foundVertInds[j].index] = i;
                    }
                }
            }
        }


        // Remove all triangles s.t. at least two of the corners in the triangle refers to the same vertex (by index)
        // NB: Does not preserve triangle order
        void removeDegenerateTriangles(Mesh& mesh) {

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
        }


        /*
         * rectifyMesh - In order to fit the mesh better to corners in the shape it is supposed to represent,
         * we will create vertices closer to those corners in the mesh
         */
        void rectifyMesh(const GeometricExpression& ge, Mesh& original_mesh, hg::HalfEdgeMesh& hem) {
            int split_edges = 0;

            unsigned int num_edges = hem.getSize();

            for (unsigned int i = 0; i < num_edges; i++) {
                hg::HalfEdge& edge = *hem.getData()[i];
                falg::Vec3 pos0 = original_mesh.positions[edge.start_index];
                falg::Vec3 pos1 = original_mesh.positions[edge.end_index];

                falg::Vec3 normal0 = original_mesh.normals[edge.start_index];
                falg::Vec3 normal1 = original_mesh.normals[edge.end_index];

                float normdot = falg::dot(normal0, normal1);

                // Vertices with higher angle between normals are chosen for edge-split
                const float cosdot = cosf(M_PI / 4);

                // Assume length of normals are 1
                // If the normals do not align well...
                if (normdot < cosdot) {


                    // If found vertex has normal with angle within this from any of the two start points,
                    // continue looking for new point (we'll try binary search)
                    const float mindot = cosf(M_PI / 16);
                    bool found_new_vertex = false;

                    const int max_search_iterations = 4;
                    int search_iterations = 0;

                    // Binary search range
                    float high_frac = 1.0f,
                        low_frac = 0.0f;

                    falg::Vec3 middle_point, new_normal;

                    while (!found_new_vertex && search_iterations < max_search_iterations) {
                        search_iterations++;

                        // Pick middle point
                        float middle_frac = (high_frac + low_frac) / 2.0f;
                        middle_point = (1 - middle_frac) * pos0 + middle_frac * pos1;

                        falg::Vec3 interp_normal = (normal0 + normal1).normalized();

                        falg::Vec3 dir = pos1 - pos0;
                        // If the two normals point away from the other vertex ...
                        if (falg::dot(dir, normal0) < 0 && falg::dot(dir, normal1) > 0) {
                            float dist = ge.signedDist(middle_point);
                            int a = 0;
                            while (dist < 0.0f && a < 4) {
                                middle_point -= dist * 1.5 * interp_normal;
                                dist = ge.signedDist(middle_point);
                                a++;
                            }

                            new_normal = ge.normal(middle_point);
                            middle_point -= dist * new_normal;

                            // Else, if normals point towards the other vertex
                        } else if (falg::dot(dir, normal0) > 0 && falg::dot(dir, normal1) < 0) {
                            float dist = ge.signedDist(middle_point);
                            int a = 0;
                            while (dist > 0.0f && a < 4) {
                                middle_point -= dist * 1.5 * interp_normal;
                                dist = ge.signedDist(middle_point);
                                a++;
                            }

                            new_normal = ge.normal(middle_point);
                            middle_point -= dist * new_normal;
                        } else {
                            // Don't really know what to do when original normals point in the same direction, just skip
                            break;
                        }

                        // Check if found a new, sufficiently distinct normal
                        if (falg::dot(new_normal, normal0) > mindot) {
                            low_frac = (high_frac + low_frac) / 2.0f;
                        } else if (falg::dot(new_normal, normal1) > mindot) {
                            high_frac = (high_frac + low_frac) / 2.0f;
                        } else {
                            // If normal sufficiently different from originals, mark as found
                            found_new_vertex = true;
                        }
                    }

                    if (!found_new_vertex) {
                        continue;
                    }

                    int this_ind = original_mesh.positions.size();
                    original_mesh.positions.push_back(middle_point);
                    original_mesh.normals.push_back(new_normal);

                    hem.splitEdge(&edge, this_ind);
                    split_edges++;
                }
            }
        }

        hg::NormalMesh constructMesh(const GeometricExpression& ge, float target_resolution, float span, const falg::Vec3& mid,
                                     bool postProcessMesh) {

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

            removeDegenerateTriangles(mesh);

            if (postProcessMesh) {
                hg::HalfEdgeMesh hem(mesh);
                rectifyMesh(ge, mesh, hem);
                hem.constructIndices(mesh.indices);
            }

            return mesh;
        }
    };
};
