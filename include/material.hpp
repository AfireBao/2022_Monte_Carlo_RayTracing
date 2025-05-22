#ifndef MATERIAL_H
#define MATERIAL_H


#include "stb_image.h"
#include "ray.hpp"
#include "hit.hpp"
#include <cassert>
#include <vecmath.h>

#include <iostream>
#include <string>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(char filename_[],const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, Vector3f e = Vector3f::ZERO, float s = 0, int type = 0, double ns_ = 1.5, int textureFunc_=0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s), Reflect_type(type), Energy(e), ns(ns_),textureFunc(textureFunc_) {
                filename = filename_;
                if(filename != ""){
                    buf = stbi_load(filename.c_str(), &w, &h, &c, 0);
                }
                else
                    buf = NULL;
    }
    Material(){}
    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }
    Vector3f getEnergy(){
        return Energy;
    }
    int getType(){
        return Reflect_type;
    }
    double getns(){
        return ns;
    }
    void settheta(double xita,double t_){
        theta = xita;
        t = t_;
    }
    Vector3f getcolor(double x_,double y_, double z_){
        //  std::cout<<"draw"<<std::endl;
        if(buf == NULL) return diffuseColor;
        double a,b;
        // background
        if(textureFunc == 1){
            a = (x_+2)/4.;
            b = (-y_+2)/4.;
        }
        // top
        if(textureFunc == 2){
            a = (x_ + 2)/ 4.;
            b = (z_ + 2)/ 4. - (int)(z_ + 2)/4.;
        }
        // bottom
        if(textureFunc == 3){
            a = (x_ + 2)/ 4.;
            b = (z_ + 2)/ 4. - (int)(z_ + 2)/4.;
        }
        // left
        if(textureFunc == 4){
            a = (y_ + 2)/4.;
            b = (z_ + 2)/ 4. - (int)(z_ + 2)/4.;
        }
        // right
        if(textureFunc == 5){
            a = (y_ + 2)/4.;
            b = (z_ + 2)/ 4. - (int)(z_ + 2)/4.;
        }
        //front
        if(textureFunc == 6){
            a = (x_+2)/4.;
            b = (-y_+2)/4.;
        }
        // revsurface
        if(textureFunc == 7){
            b = (5*t) - (int)(5*t);
            a = theta;
        }
        int pw = (int(a * w) % w + w) % w, ph = (int(b * h) % h + h) % h;
        int idx = ph * w * c  + pw * c;
        int x = buf[idx + 0], y = buf[idx + 1],  z = buf[idx + 2];
        return Vector3f(x,y,z)/255.;
    }
    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        //
        Vector3f Li = dirToLight.normalized();
        Vector3f N = hit.getNormal().normalized();
        Vector3f V = -ray.getDirection().normalized();
        Vector3f Ri = 2*Vector3f::dot(N,Li)*N-Li;
        float cla1,cla2;
        cla1 = Vector3f::dot(Li,N);
        cla2 = Vector3f::dot(V,Ri);
        cla1 = cla1>0?cla1:0;
        cla2 = cla2>0?cla2:0;
        shaded = lightColor*(cla1*diffuseColor + pow(cla2,shininess) * specularColor);
        return shaded;
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    Vector3f Energy;
    float shininess;
    int Reflect_type;
    double ns;
    std::string filename;
    unsigned char* buf;
    int w, h, c;
    int textureFunc;
    double theta,t;
};


#endif // MATERIAL_H
