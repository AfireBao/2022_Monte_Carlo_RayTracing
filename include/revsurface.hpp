#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include <tuple>
#include "box.hpp"
typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;

class RevSurface : public Object3D {

    BezierCurve *pCurve;
    Box box_;
public:
    struct Surface {
            std::vector<Vector3f> VV;
            std::vector<Vector3f> VN;
            std::vector<Tup3u> VF;
        } surface;
    RevSurface(BezierCurve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        double ymin = pCurve->low();
        double ymax = pCurve->high();
        double xmax = pCurve->right(); 
        box_ = Box(-0.99,0.99,-1.99,ymax,-0.99,0.99,material);
    }
        // std::vector<CurvePoint> curvePoints;
        // pCurve->discretize(5, curvePoints);
        // const int steps = 10;
        // for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
        //     const CurvePoint &cp = curvePoints[ci];
        //     for (unsigned int i = 0; i < steps; ++i) {
        //         float t = (float) i / steps;
        //         Quat4f rot;
        //         rot.setAxisAngle(t * 2 * 3.14159, Vector3f::UP);
        //         Vector3f pnew = Matrix3f::rotation(rot) * cp.V;
        //         Vector3f pNormal = Vector3f::cross(cp.T, -Vector3f::FORWARD);
        //         Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
        //         surface.VV.push_back(pnew);
        //         surface.VN.push_back(nnew);
        //         int i1 = (i + 1 == steps) ? 0 : i + 1;
        //         if (ci != curvePoints.size() - 1) {
        //             surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
        //             surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
        //         }
        //     }
        // }

    ~RevSurface() override {
        delete pCurve;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        if(!box_.intersect(r,h,tmin))return false;
        // cout<<"enter"<<endl;
        bool isintersect = false;
        for(int i=1;i<=10;++i){
            double t_ = 0.1 * i;
            if(intersectByt(r,h,tmin,t_)){
                isintersect = true;
            }
        }
        if(isintersect) return true;
        return false;
    }

    // bool intersect(const Ray &r, Hit &h, float tmin){
    //     bool isintersect = false;
    //     for(int i=0;i<surface.VF.size();++i){
    //         Vector3f t1 = surface.VV[get<0>(surface.VF[i])];
    //         Vector3f t2 = surface.VV[get<1>(surface.VF[i])];
    //         Vector3f t3 = surface.VV[get<2>(surface.VF[i])];
    //         Triangle tr = Triangle(t1,t2,t3,material);
    //         if(tr.intersect(r,h,tmin)){
    //             isintersect = true;
    //         }
    //     }
    //     if(isintersect) {
    //         // std::cout<<material->getDiffuseColor().x();
    //         return true;
    //     }
    //     return false;
    // }

    bool intersectByt(const Ray &r, Hit &h, float tmin,double t_){
        int max_times = 20;
        double min_step = 1e-4;
        double t = t_;
        double O_x = r.getOrigin().x();
        double O_y = r.getOrigin().y();
        double O_z = r.getOrigin().z();
        double D_x = r.getDirection().x();
        double D_y = r.getDirection().y();
        double D_z = r.getDirection().z();
        // D_y -> 0
        if(abs(D_y) <= 1e-6){
            return false;
            for(int i=0;i<max_times;++i){
                if(t < 0 || t > 1) return false;
                CurvePoint curvepoint = pCurve->point(t);
                double x_t = curvepoint.V.x();
                double y_t = curvepoint.V.y();
                double z_t = curvepoint.V.z();
                double dx_t = curvepoint.T.x();
                double dy_t = curvepoint.T.y();
                double f_t = y_t - O_y;
                double df_t = dy_t;
                if(abs(f_t) <= 1e-2){
                    double A = D_x*D_x + D_z*D_z;
                    double B = 2 * (O_x * D_x + O_z * D_z);
                    double C = O_x*O_x +  O_z*O_z - x_t * x_t;
                    double delta = B * B - 4 * A * C;
                    if(delta < 0) return false;
                    double tr = (-B + sqrt(delta)) / (2 * A);
                    double x_ = tr * D_x + O_x;
                    double y_ = y_t;
                    double z_ = tr * D_y + O_z;
                    Vector3f surfacePoint = Vector3f(x_,y_,z_);
                    double xita = atan2(-z_,x_);
                    Quat4f rot;
                    rot.setAxisAngle(xita, Vector3f::UP);
                    Vector3f pNormal = Vector3f::cross(curvepoint.T, Vector3f::FORWARD);
                    Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
                    if(tr>tmin && tr < h.getT()){
                        h.set(tr,material,nnew.normalized());
                        return true;
                    }
                    return false;
                }
            }
        }
        for(int i=0;i<max_times;++i){
            if( t < 0 || t > 1)return false;
            // std::cout<<"t: "<<t<<endl;
            CurvePoint curvepoint = pCurve->point(t);
            double x_t = curvepoint.V.x();
            double y_t = curvepoint.V.y();
            double z_t = curvepoint.V.z();
            // std::cout<<"x: "<<x_t<<" y: "<<y_t<<" z: "<<z_t<<std::endl;
            double dx_t = curvepoint.T.x();
            double dy_t = curvepoint.T.y();
            double x_ = (y_t - O_y) * D_x / D_y + O_x;
            double y_ = y_t;
            double z_ = (y_t - O_y) * D_z / D_y + O_z;
            double f_t = (x_t * x_t) - ((y_t - O_y)* (y_t - O_y) * (D_x*D_x + D_z*D_z) / (D_y*D_y) +
                        2 * (y_t - O_y) * (D_x * O_x + D_z * O_z) / D_y + O_x * O_x + O_z * O_z);
            double df_t = 2 * x_t * dx_t - (2 * (y_t - O_y) * dy_t *(D_x*D_x + D_z * D_z) / (D_y*D_y) +
                        2 * dy_t * (D_x * O_x + D_z * O_z) / D_y);
            if(abs(f_t) <= 1e-2){
                float tr = (y_ - O_y) / D_y;
                Vector3f surfacePoint = Vector3f(x_,y_,z_);
                double xita = atan2(-z_,x_);
                Quat4f rot;
                rot.setAxisAngle(xita, Vector3f::UP);
                Vector3f pNormal = Vector3f::cross(curvepoint.T, Vector3f::FORWARD);
                Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
                // cout<<"D_x: "<<x_<<" Rotate_x: "<< (Matrix3f::rotation(rot) * curvepoint.V).x()<<endl;
                // cout<<"D_y: "<<y_<<" Rotate_y: "<< (Matrix3f::rotation(rot) * curvepoint.V).y()<<endl;
                // cout<<"D_z: "<<z_<<" Rotate_z: "<< (Matrix3f::rotation(rot) * curvepoint.V).z()<<endl;
                // cout<<"tr: "<<tr<<" y: "<<y_<<" O_y: "<<O_y<<endl;
                if(tr>tmin && tr < h.getT()){
                    material->settheta(xita,tr);
                    h.set(tr,material,nnew.normalized());
                    return true;
                }
                return false;
            }
            
            double newt = t - f_t / df_t;
            if(abs(newt - t) < min_step)  return false;
            t = newt;
        }
        // std::cout<<"too many"<<std::endl;
        return false;
    }
};

#endif //REVSURFACE_HPP
