#ifndef BVHTREE_H
#define BVHTREE_H

#include "object3d.hpp"
#include "triangle.hpp"
#include "box.hpp"
#include <vecmath.h>
#include <cmath>
#include <vector>

struct bvhNode{
    int leftchild,rightchild;
    Box box;
    vector<Triangle> Tset;
    bvhNode(int l,int r,Box box_, vector<Triangle> T){
        leftchild = l;
        rightchild = r;
        box = box_;
        for(int i=0;i<T.size();++i){
            Tset.push_back(T[i]);
        }
    }
    bvhNode(){
        leftchild = -1;
        rightchild = -1;
        box = Box();
    }
    bool intersect(const Ray &r, Hit &h, float tmin) {
        bool isintersect = false;
        for(int i=0;i<Tset.size();++i){
            if(Tset[i].intersect(r,h,tmin))isintersect = true;
        }
        return isintersect;   
    }
};

class bvhTree{
    private:
        int root;
        bvhNode tree[1000];
        
    public:
        bvhTree(){root = 0;}
        bvhTree(int num){ root = 0;}
        ~bvhTree(){
        }

        // mutiple thread forbidden
        void init(vector<Triangle> & Triangles, int num, int start, int end){
            // printf("%d,%d\n",start,end);
            // leaf node
            if((end-start) <= 5){
                Material m = Material();
                Box box_ = Box(xmin(Triangles,start,end),xmax(Triangles,start,end),ymin(Triangles,start,end),ymax(Triangles,start,end),zmin(Triangles,start,end),zmax(Triangles,start,end),&m);
                vector<Triangle> tset;
                for(int i=start;i<end;i++){
                    tset.push_back(Triangles[i]);
                }
                bvhNode node = bvhNode(-1,-1,box_,tset);
                tree[num] = node;
                return;
            }
            // parent node
            Material m = Material();
            Box box_ = Box(xmin(Triangles,start,end),xmax(Triangles,start,end),ymin(Triangles,start,end),ymax(Triangles,start,end),zmin(Triangles,start,end),zmax(Triangles,start,end),&m);
            vector<Triangle> hollow;
            tree[num] = bvhNode(2*num+1,2*num+2,box_,hollow);

            double xlen = xmax(Triangles,start,end) - xmin(Triangles,start,end);
            double ylen = ymax(Triangles,start,end) - ymin(Triangles,start,end);
            double zlen = zmax(Triangles,start,end) - zmin(Triangles,start,end);
            // x:1,y:2,z:3
            int divideBy;
            if(xlen > ylen && xlen > zlen) divideBy = 1;
            else if(ylen > xlen && ylen > zlen) divideBy = 2;
            else divideBy = 3;
            // for(int i=start;i<end;++i){
            //     printf("%lf,",max(Triangles[i].vertices[0].x(),Triangles[i].vertices[1].x(),Triangles[i].vertices[2].x()));
            // }
            // cout<<endl;
            Bubblesort(Triangles,divideBy,start,end);

            int mid = start + (end-start) / 2;
            init(Triangles,2*num+1,start,mid);
            init(Triangles,2*num+2,mid,end);
        }

        bool intersect(const Ray &r, Hit &h, float tmin, int num) {
            bool isintersect = false;
            // leaf node
            if(tree[num].leftchild == -1 && tree[num].rightchild == -1){
                if(!tree[num].box.intersect(r,h,tmin))return false;
                return tree[num].intersect(r,h,tmin);
            }
            // parent node
            if(!tree[num].box.intersect(r,h,tmin))return false;
            Hit h1=h,h2=h;
            if(tree[num].leftchild > 0) isintersect |= intersect(r,h1,tmin,2*num+1);
            if(tree[num].rightchild > 0) isintersect |= intersect(r,h2,tmin,2*num+2);
            if(isintersect){
                if(h1.getT()<h2.getT()){
                    h.set(h1.getT(),h1.getMaterial(),h1.getNormal());
                }
                else{
                    h.set(h2.getT(),h2.getMaterial(),h2.getNormal());
                }
                return true;
            }
           
            return false;
        }

        // utils    
        void Bubblesort(vector<Triangle>& t,int xyz,int start, int end){
            for(int i=start;i<end;++i){
                for(int j=start;j<end- 1 -i;++j){
                    if(!smaller(t[j],t[j+1],xyz)){
                        Triangle tmp = t[j+1];
                        t[j+1] = t[j];
                        t[j] = tmp;
                    }
                }
            }
        }
        bool smaller(Triangle t1,Triangle t2,int xyz){
            if(xyz == 1){
                double t1max = max(t1.vertices[0].x(),t1.vertices[1].x(),t1.vertices[2].x());
                double t2max = max(t2.vertices[0].x(),t2.vertices[1].x(),t2.vertices[2].x());
                return t1max < t2max ? true : false;
            }
            if(xyz == 2){
                double t1max = max(t1.vertices[0].y(),t1.vertices[1].y(),t1.vertices[2].y());
                double t2max = max(t2.vertices[0].y(),t2.vertices[1].y(),t2.vertices[2].y());
                return t1max < t2max ? true : false;
            }
            if(xyz == 3){
                double t1max = max(t1.vertices[0].z(),t1.vertices[1].z(),t1.vertices[2].z());
                double t2max = max(t2.vertices[0].z(),t2.vertices[1].z(),t2.vertices[2].z());
                return t1max < t2max ? true : false;
            }
        }
        double max(double a,double b,double c){
            if(a > b && a > c)return a;
            if(b > a && b > c)return b;
            return c;
        }
        double min(double a,double b,double c){
            if(a < b && a < c)return a;
            if(b < a && b < c)return b;
            return c;
        }
        double xmax(const vector<Triangle>&Triangles, int s, int e){
            double xmax = -1e12;
            for(int i=s;i<e;++i){
                double tmax = max(Triangles[i].vertices[0].x(),Triangles[i].vertices[1].x(),Triangles[i].vertices[2].x());
                if( tmax > xmax){
                    xmax = tmax;
                }
            }
            return xmax;
        }
        double ymax(const vector<Triangle>&Triangles, int s, int e){
            double ymax = -1e12;
            for(int i=s;i<e;++i){
                double tmax = max(Triangles[i].vertices[0].y(),Triangles[i].vertices[1].y(),Triangles[i].vertices[2].y());
                if( tmax > ymax){
                    ymax = tmax;
                }
            }
            return ymax;
        }
        double zmax(const vector<Triangle>&Triangles, int s, int e){
            double zmax = -1e12;
            for(int i=s;i<e;++i){
                double tmax = max(Triangles[i].vertices[0].z(),Triangles[i].vertices[1].z(),Triangles[i].vertices[2].z());
                if( tmax > zmax){
                    zmax = tmax;
                }
            }
            return zmax;
        }
        double xmin(const vector<Triangle>&Triangles, int s, int e){
            double xmin = 1e12;
            for(int i=s;i<e;++i){
                double tmin = min(Triangles[i].vertices[0].x(),Triangles[i].vertices[1].x(),Triangles[i].vertices[2].x());
                if( tmin < xmin){
                    xmin = tmin;
                }
            }
            return xmin;
        }
        double ymin(const vector<Triangle>&Triangles, int s, int e){
            double ymin = 1e12;
            for(int i=s;i<e;++i){
                double tmin = min(Triangles[i].vertices[0].y(),Triangles[i].vertices[1].y(),Triangles[i].vertices[2].y());
                if( tmin < ymin){
                    ymin = tmin;
                }
            }
            return ymin;
        }
        double zmin(const vector<Triangle>&Triangles, int s, int e){
            double zmin = 1e12;
            for(int i=s;i<e;++i){
                double tmin = min(Triangles[i].vertices[0].z(),Triangles[i].vertices[1].z(),Triangles[i].vertices[2].z());
                if( tmin < zmin){
                    zmin = tmin;
                }
            }
            return zmin;
        }
        
};

#endif //BVHTREE_H
		

