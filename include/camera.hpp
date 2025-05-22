#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
    float angle;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        this->angle = angle;
    }

    Ray generateRay(const Vector2f &point) override {
        //尺度参数
        float fx,fy;
        fx= fy=height/(2*tan(angle/2));
        //光心坐标
        int cx,cy;
        cx = width/2;
        cy = height/2;
        //相机空间
        Vector3f ORe = Vector3f(0,0,0);
        Vector3f dRe = Vector3f((point.x()-cx)/fx,(cy-point.y())/fy,1);
        //世界空间
        Vector3f ORw = center;
        Matrix3f R = Matrix3f(horizontal,-up,direction);
        Vector3f dRw = R*dRe;
        return Ray(ORw,dRw);
    }
};

#endif //CAMERA_H
