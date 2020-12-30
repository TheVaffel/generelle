#include <FlatAlg.hpp>
#include "../include/generelle/modelling.hpp"

namespace generelle {

    /*
     * Ray struct
     */

    struct Ray {
        falg::Vec3 dir;
        falg::Vec3 origin;

        Ray(const falg::Vec3& origin,
            const falg::Vec3& dir);
    };


    /*
     * Functions for visualization
     */

    class Visualization {
        struct RayMarchResult {
            bool hit;
            float t;
            falg::Vec3 pos;
            falg::Vec3 color;
            falg::Vec3 normal;
        };

        Visualization() = delete;

        static RayMarchResult rayMarch(const Ray& ray, const GE& geom);

    public:

        static unsigned char* visualize(const GE& expr, int width, int height);
        static void destroyBuffer(unsigned char* ch);
    };
};
