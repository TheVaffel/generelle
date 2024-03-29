#include "mesh_constructor.hpp"

#include "marching_cubes.hpp"

#include <HGraf.hpp>

namespace generelle {

    namespace MeshConstructor {


        

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
         * reprojectMesh - make sure each vertex on the surface lies on the GE boundary (e.g. signedDist = 0)
         */
        void reprojectMesh(const GeometricExpression& ge, Mesh& original_mesh) {
            for (unsigned int i = 0; i < original_mesh.positions.size(); i++) {
                float dist = ge.signedDist(original_mesh.positions[i]);
                original_mesh.positions[i] -= original_mesh.normals[i] * dist;
            }
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
                        float length = dir.norm();

                        float factor;

                        // If the two normals point away from the other vertex ...
                        if (falg::dot(dir, normal0) < 0 && falg::dot(dir, normal1) > 0) {
                            factor = -1.0f;
                        } else if (falg::dot(dir, normal0) > 0 && falg::dot(dir, normal1) < 0) {
                            factor = 1.0f;
                        } else {
                            // Don't really know what to do when original normals point in the same direction, just skip
                            break;

                        }

                        float padded_dist = ge.signedDist(middle_point) + factor * length / 4;
                        int a = 0;
                        while (factor * padded_dist > 0 && a < 4) {
                            middle_point -= padded_dist * 1.5 * interp_normal;
                            padded_dist = ge.signedDist(middle_point) + factor * length / 4;
                            a++;
                        }

                        new_normal = ge.normal(middle_point);
                        float new_dist = padded_dist - factor *  length / 4;
                        middle_point -= new_dist * new_normal;

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
                    original_mesh.normals.push_back(ge.normal(middle_point));

                    hem.splitEdge(&edge, this_ind);
                    split_edges++;

                }
            }
        }

        void simplifyMesh(Mesh& original_mesh, hg::HalfEdgeMesh& hem, float max_len) {
            float mls = max_len * max_len;

            for (unsigned int i = 0; i < hem.getSize(); i++) {
                hg::HalfEdge* edge =  hem.getData()[i];
                int v0 = edge->start_index;
                int v1 = edge->end_index;

                falg::Vec3 norm0 = original_mesh.normals[v0];
                falg::Vec3 norm1 = original_mesh.normals[v1];
                falg::Vec3 pdiff = original_mesh.positions[v1] - original_mesh.positions[v0];
                float ddot = falg::dot(norm0, pdiff);

                float lk = std::abs(ddot / pdiff.norm());

                if ((norm0 - norm1).sqNorm() < 1e-10 && lk < 1e-7 && pdiff.sqNorm() < mls) {
                    hem.mergeEdge(edge);
                    original_mesh.positions[v0] = (original_mesh.positions[v0] + original_mesh.positions[v1]) / 2;
                    // We leave the deleted position in the mesh, delete on reconstruction
                    // Normals will be the same as before
                }
            }
        }

        hg::NormalMesh constructMesh(const GeometricExpression& ge, float target_resolution, float span, const falg::Vec3& mid,
                                     const ConstructMeshSetup& setup) {

            std::vector<falg::Vec3> temp_positions;
	    MarchingCubes::marchingCubes(ge,
					 temp_positions,
					 target_resolution / 2, span, mid);


            std::vector<int> indMap;
            deduplicateMapPoints(temp_positions, indMap, 1e-3);

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


            reprojectMesh(ge, mesh);
            hg::HalfEdgeMesh hem(mesh);
            for (int i = 0; i < setup.numRectify; i++) {
                rectifyMesh(ge, mesh, hem);
            }

            if (setup.includeSimplify) {
                simplifyMesh(mesh, hem, target_resolution * 2);
            }

            hem.reconstructMesh(mesh);

            return mesh;
        }
    };
};
