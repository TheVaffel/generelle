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

        float eps = 1e-3;
        
        float t = 0;
        falg::Vec3 currPos = ray.origin + ray.dir * t;
        float dist = 1e9;
        for (int i = 0; i < 100; i++) {
            dist = geom.signedDist(currPos);
            t += dist; // * 1.2f;
            currPos = ray.origin + ray.dir * t;

            if (dist < eps || t > 100) {
                break;
            }
        }

        Visualization::RayMarchResult res;
        if (dist < eps) {
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

        falg::Vec3 campos(-3.0f, 3.0f, -3.0f);
        falg::Vec4 cam_up(0.0f, 1.0f, 0.0f, 0.0f);
        falg::Vec4 cam_forward(0.0f, 0.0f, 1.0f, 0.0f);
        falg::Vec4 cam_right(1.0f, 0.0f, 0.0f, 0.0f);

        float width_r = 1.0f;
        float height_r = (width_r * height) / width;

        unsigned char* ch = new unsigned char[width * height * 4];

        
        falg::Mat4 viewMatrix(falg::FLATALG_MATRIX_LOOK_AT,
                              campos,
                              falg::Vec3(0.0f, 0.0f, 0.0f),
                              falg::Vec3(0.0f, 1.0f, 0.0f));
        
        // viewMatrix is from world to camera space. We want the other direction (ignore translation part)
        viewMatrix = ~viewMatrix;

        falg::Vec4 up4 = viewMatrix * cam_up;
        falg::Vec4 forward4 = viewMatrix * cam_forward;
        falg::Vec4 right4 = viewMatrix * cam_right;

        falg::Vec3 up = falg::Vec3(up4.x(), up4.y(), up4.z());
        falg::Vec3 forward = - falg::Vec3(forward4.x(), forward4.y(), forward4.z());
        falg::Vec3 right = falg::Vec3(right4.x(), right4.y(), right4.z());
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {

                float width_c = 2 * (j - width / 2 + 0.5f) / width * width_r;
                float height_c = - 2 * (i - height / 2 + 0.5f) / height * height_r;

                Ray ray(campos, forward + up * height_c + right * width_c);
                ray.dir = ray.dir.normalized();

                RayMarchResult res = rayMarch(ray, expr);

                falg::Vec3 color = res.color;
                if (res.hit) {
                    falg::Vec3 sun_dir = falg::Vec3(3.0f, -2.0f, 1.0f).normalized();
                    color = color * std::max(( - falg::dot(sun_dir, res.normal)), 0.0f);
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
