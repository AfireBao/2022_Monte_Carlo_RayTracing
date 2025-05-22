#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

//Implement functions and add more fields as necessary
#define eps 1e-6
class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        // 
        center_ = center;
        radius_ = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // Vector3f ro = center_ - r.getOrigin();
        // double b = Vector3f::dot(ro,r.getDirection());
        // double d = b * b - Vector3f::dot(ro,ro) + radius_*radius_;
        // if(d < 0 ) return false;
        // else d = sqrt(d);
        // double t = b-d>eps ? b-d : b+d>eps? b+d : 0;
        // Vector3f norm = r.getOrigin() + t*r.getDirection() - center_;
        // if(t > tmin && t < h.getT()){
        //     // if(t > 100){
        //     //             std::cout<<"t is large"<<std::endl;
        //     //             std::cout<<"t: "<<t<<std::endl;
        //     //             std::cout<<"Origin: "<<"x: "<<r.getOrigin().x()<<"y: "<<r.getOrigin().y()<<"z: "<<r.getOrigin().z()<<std::endl;
        //     //             std::cout<<"Direction: "<<"x: "<<r.getDirection().x()<<"y: "<<r.getDirection().y()<<"z: "<<r.getDirection().z()<<std::endl;
        //     // }
        //     h.set(t,material,norm.normalized());
        //     return true;
        // }
        // return false;
        //视线原点到球心的向量l
        Vector3f l = center_-r.getOrigin();
        float L2 =  center_.dot(l,l);
        //球心到视线距离d
        Vector3f rDirection = r.getDirection();
        float RDirection2 = rDirection.dot(rDirection,rDirection);
        float D2 = L2*(1-(l.dot(l,rDirection)*l.dot(l,rDirection))/(L2*RDirection2));
        float R2 = radius_ * radius_;
        //视线原点到垂点距离tp
        float tp = rDirection.dot(rDirection,l)/sqrt(RDirection2);
        float t;
        //当d>r时，视线与球面无交点
        if(D2 > R2){
            return false;
        }
        else{
            //当d=r时，视线与球面相切
            if(D2 == R2){
                t = tp;
            }
            //当d<r时，视线与球相交
            else{
                float t_ = sqrt(R2-D2);
                //当l>r时，视线原点在球面外
                if(L2>R2){
                    t = tp - t_;
                }
                //当l<=r时，视线原点在球面内
                else{
                    t = tp + t_;
                }
                t = t /sqrt(RDirection2);
            }
            //t<0直接舍弃
            if(t < 0){
                    return false;
                }
            else{
                Vector3f norm = r.getOrigin() + t*r.getDirection() - center_;
                if(t >tmin && t<h.getT()){
                    h.set(t,material,norm.normalized());
                    // if(t > 100){
                    //     std::cout<<"t is large"<<std::endl;
                    //     std::cout<<"t: "<<t<<std::endl;
                    //     std::cout<<"Origin: "<<"x: "<<r.getOrigin().x()<<"y: "<<r.getOrigin().y()<<"z: "<<r.getOrigin().z()<<std::endl;
                    //     std::cout<<"Direction: "<<"x: "<<r.getDirection().x()<<"y: "<<r.getDirection().y()<<"z: "<<r.getDirection().z()<<std::endl;
                    // }
                    return true;
                }
                else{
                    return false;
                }
            }
        }
        
    }

protected:
    Vector3f center_;
    float radius_;
};


#endif
