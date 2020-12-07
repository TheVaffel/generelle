#include "visualization.hpp"

#include <iostream>

namespace generelle {
    
    /*
     * Ray member functions
     */

    Ray::Ray(const falg::Vec3& origin, const falg::Vec3& direction) : dir(direction), origin(origin) { }

    /*
     * Visualization static methods
     */
    
    Visualization::RayMarchResult Visualization::rayMarch(const Ray& ray, const GE& geom) {
        float t = 0;
        falg::Vec3 currPos = ray.origin + ray.dir * t;
        float dist = 1e9;
        for (int i = 0; i < 100; i++) {
            dist = geom.signedDist(currPos);
            t += dist; // * 1.2f;
            currPos = ray.origin + ray.dir * t;

            if (dist < 1e-2 || t > 100) {
                break;
            }
        }

        Visualization::RayMarchResult res;
        if (dist < 1e-2) {
            res.hit = true;
            res.color = falg::Vec3(1.0f, 1.0f, 1.0f);
            res.t = t;
            res.pos = currPos;
            res.normal = geom.normal(currPos);
        } else {
                res.hit = false;
            res.color = falg::Vec3(0.0f, 0.0f, 0.0f);
            res.t = t;
        }

        return res;
    }

    unsigned char* Visualization::visualize(const GE& expr, int width, int height) {

        falg::Vec3 campos(0.0f, 0.0f, -3.0f);
        falg::Vec3 up(0.0f, 1.0f, 0.0f);
        falg::Vec3 forward(0.0f, 0.0f, 1.0f);
        falg::Vec3 right(1.0f, 0.0f, 0.0f);

        float width_r = 1.0f;
        float height_r = (width_r * height) / width;

        unsigned char* ch = new unsigned char[width * height * 4];
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                float width_c = 2 * (j - width / 2 + 0.5f) / width * width_r;
                float height_c = - 2 * (i - height / 2 + 0.5f) / height * height_r;

                Ray ray(campos, forward + up * height_c + right * width_c);
                ray.dir = ray.dir.normalized();
                        

                RayMarchResult res = rayMarch(ray, expr);

                falg::Vec3 color = res.color;
                if (res.hit) {
                    falg::Vec3 sun_dir = falg::Vec3(1.0f, -1.0f, 1.0f).normalized();
                    color = color * ( - falg::dot(sun_dir, res.normal));
                }
                
                int base_ind = 4 * (i * width + j);
                for (int k = 0; k < 3; k++) {
                    ch[base_ind + k] = std::max(std::min(255.0f, color[k] * 255), 0.0f);
                }
                ch[base_ind + 3] = 255;
            }
        }

        return ch;
    }

    void Visualization::destroyBuffer(unsigned char* ch) {
        delete[] ch;
    }
};
