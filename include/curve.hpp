#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

#define inf 1e10;
// TODO (PA2): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {
        
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        std::cout<<"curve"<<std::endl;
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    long long int Hierarchy(int n){
        if(n == 0)return 1;
        int ret = n;
        for(int i=n-1;i>0;i--){
            ret *= i;
        }
        return ret;
    }

    double Combination(int n, int m){
        if(n<m)return 0.;
        return (double)Hierarchy(n)/(double)(Hierarchy(m)*Hierarchy(n-m));
    }

    double Bezier(int i,int k,double t){
        return Combination(k,i) * pow(1-t,k-i) * pow(t,i);
    }

    double Bezier_(int i,int k,double t){
        if(i==0)return -k*pow(1-t,k-1);
        return k * (Bezier(i-1,k-1,t)-Bezier(i,k-1,t));
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        int n = controls.size() - 1;
        double interaction = 1./resolution;
        double t = 0;
        for(int j=0;j<resolution;t+=interaction,j++){
            CurvePoint point;
            point.V = Bezier(0,n,t) * controls[0];
            point.T = Bezier_(0,n,t) * controls[0];
            for(int i=1;i<=n;i++){
                point.V += Bezier(i,n,t) * controls[i];
                point.T += Bezier_(i,n,t) * controls[i];
            }
            data.push_back(point);
        }
    }

    CurvePoint point(double t){
        int n = controls.size() - 1;
        CurvePoint point;
        point.V = Bezier(0,n,t) * controls[0];
        point.T = Bezier_(0,n,t) * controls[0];
        for(int i=1;i<=n;i++){
            point.V += Bezier(i,n,t) * controls[i];
            point.T += Bezier_(i,n,t) * controls[i];
        }
        return point;
    }

    double high(){
        double ret = -inf;
        for(int i=0;i<controls.size();++i){
            if(controls[i].y() > ret) ret = controls[i].y();
        }
        return ret;
    }

    double low(){
        double ret = inf;
         for(int i=0;i<controls.size();++i){
            if(controls[i].y() < ret) ret = controls[i].y();
        }
        return ret;
    }

    double right(){
        double ret = -inf;
        for(int i=0;i<controls.size();++i){
            if(controls[i].x() > ret) ret = controls[i].x();
        }
        return ret;
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        // for(int i=0;i<k;i++){
        //     controls[i] = controls[points.size() - k + i];
        // }
    }
    float ti(int i){
        return float(i)/float(n+k+1);
    }
    float Boor(int i,int p,float t){
        if(p == 0){
            if(t>=ti(i) && t<ti(i+1)) {
                Boor_Cox[i][p] = 1;
                is_calculate[i][p] = true;
                return 1;
            }
            else{
                Boor_Cox[i][p] = 0;
                is_calculate[i][p] = true;
                return 0;
            }
        }
        else{
            if(is_calculate[i][p]) return Boor_Cox[i][p];
            else{
                Boor_Cox[i][p] = Boor(i,p-1,t) * (t-ti(i))/(ti(i+p)-ti(i)) 
                                + Boor(i+1,p-1,t) * (ti(i+p+1)-t)/(ti(i+p+1)-ti(i+1));
                is_calculate[i][p] =  true;
                return Boor_Cox[i][p];
            }
        }
    }
    float Boor_(int i,int p,float t){
        if(p == 0){
            return 0;
        }
        else{
            float ret = p * (Boor(i,p-1,t) / (ti(i+p) - ti(i)) - Boor(i+1,p-1,t) / (ti(i+p+1) - ti(i+1)));
            return ret;
        }
    }
    void init(){
        for(int temp1=0;temp1<30;temp1++){
            for(int temp2=0;temp2<4;temp2++){
                is_calculate[temp1][temp2] = false;
            }
        }
    }
    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        for(int m=k;m<n+1;m++){
            float interaction = ti(1)/resolution;
            float t = ti(m);
            for(int j=0;j<resolution;t+=interaction,j++){
                CurvePoint point;
                init();
                point.V = Boor(0,k,t) * controls[0];
                point.T = Boor_(0,k,t) * controls[0];
                for(int i=1;i<=n;i++){
                    point.V += Boor(i,k,t) * controls[i];
                    point.T += Boor_(i,k,t) * controls[i];
                }
                data.push_back(point);
            }
        }
        
    }

protected:
    int k = 3;
    int n = controls.size() - 1;
    float Boor_Cox[30][4];
    bool is_calculate[30][4];
};

#endif // CURVE_HPP
