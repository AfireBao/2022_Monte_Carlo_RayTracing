#ifndef BOX_H
#define BOX_H

#include "object3d.hpp"
#include "plane.hpp"
#include <vecmath.h>
#include <cmath>
#include <vector>

class Box : public Object3D {
public:
    Box() {

    }

    Box(float xmin,float xmax,float ymin,float ymax,float zmin,float zmax,Material *m) : Object3D(m) {
        AABB.push_back(Vector3f(xmin,ymin,zmin));
        AABB.push_back(Vector3f(xmax,ymax,zmax));
        // left,right,up,down,forward,back
        normals.push_back(Vector3f(-1,0,0));
        normals.push_back(Vector3f(1,0,0));
        normals.push_back(Vector3f(0,-1,0));
        normals.push_back(Vector3f(0,1,0));
        normals.push_back(Vector3f(0,0,-1));
        normals.push_back(Vector3f(0,0,1));
    }

    ~Box() override = default;

    double max(double a,double b,double c){
        if( a > b && a > c)return a;
        else if (b > a && b > c)return b;
        else return c;
    }
    double min(double a,double b,double c){
        if( a < b && a < c) return a;
        else if(b < a && b < c) return b;
        else return c;
    }
    double max(double a,double b){
        return a < b ? b : a;
    }
    double min(double a,double b){
        return a < b ? a : b;
    }
    bool intersect(const Ray &r, Hit &h, float tmin) override {
        double t_min = tmin;
        double t_max = 1e32;
        double O_x = r.getOrigin().x();
        double O_y = r.getOrigin().y();
        double O_z = r.getOrigin().z();
        double D_x = r.getDirection().x();
        double D_y = r.getDirection().y();
        double D_z = r.getDirection().z();
        double t1,t2,t3,t4,t5,t6;

        if(abs(D_x) < 1e-6){
            if (O_x < AABB[0].x() || O_x > AABB[1].x())
            {
                return false;
            }  
        }
        else{
            t1 = (AABB[0].x() - O_x) / D_x;
            t2 = (AABB[1].x() - O_x) / D_x;
            double tlower = min(t1,t2);
            double thigher = max(t1,t2);
            if(tlower > t_min) t_min = tlower;
            if(thigher < t_max) t_max = thigher;
            if(t_min > t_max) return false;
        }
        
        if(abs(D_y) < 1e-6){
            if (O_y < AABB[0].y() || O_y > AABB[1].y())
            {
                return false;
            }  
        }
        else{
            t3 = (AABB[0].y() - O_y) / D_y;
            t4 = (AABB[1].y() - O_y) / D_y;
            double tlower = min(t3,t4);
            double thigher = max(t3,t4);
            if(tlower > t_min) t_min = tlower;
            if(thigher < t_max) t_max = thigher;
            if(t_min > t_max) return false;
        }
        
        if(abs(D_z) < 1e-6){
            if (O_z < AABB[0].z() || O_z > AABB[1].z())
            {
                return false;
            }  
        }
        else{
            t5 = (AABB[0].z() - O_z) / D_z;
            t6 = (AABB[1].z() - O_z) / D_z;
            double tlower = min(t5,t6);
            double thigher = max(t5,t6);
            if(tlower > t_min) t_min = tlower;
            if(thigher < t_max) t_max = thigher;
            if(t_min > t_max) return false;
        }

        if(t_min > tmin && t_min < h.getT()){
            // Vector3f norm;
            // if(t_min == t1)norm = normals[0];
            // if(t_min == t2)norm = normals[1];
            // if(t_min == t3)norm = normals[2];
            // if(t_min == t4)norm = normals[3];
            // if(t_min == t5)norm = normals[4];
            // if(t_min == t6)norm = normals[5];
            // h.set(t_min,material,norm);
            return true;
        }
        return false;
        
        
    }


protected:
    std::vector<Vector3f> AABB;
    std::vector<Vector3f> normals;
};

#endif //BOX_H
		

