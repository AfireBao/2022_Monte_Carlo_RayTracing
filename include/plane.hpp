#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        norm = normal;
        D = -d;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        float t = -(D+Vector3f::dot(norm,r.getOrigin()))/Vector3f::dot(norm,r.getDirection());
        if(t <= 0){
            return false;
        }
        else{
            if (t > tmin && t < h.getT()) {
                h.set(t, material, norm.normalized());
                // Vector3f trr = r.pointAtParameter(t);
                // printf("%lf,%lf,%lf\n",trr.x(),trr.y(),trr.z());
                return true;
			} 
            else{
                return false;
            }
        }
        
    }

protected:
    Vector3f norm;
    float D;

};

#endif //PLANE_H
		

