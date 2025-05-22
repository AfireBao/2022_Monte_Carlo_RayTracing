#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>


bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {
    // return false;
    return tree.intersect(r,h,tmin,0);
    //  double xmax = -1e12,ymax = -1e12, zmax = -1e12, xmin = 1e12, ymin = 1e12 , zmin = 1e12;
    // for(int i=0;i<v.size();++i){
    //     if(v[i].x() < xmin) xmin = v[i].x();
    //     if(v[i].y() < ymin) ymin = v[i].y(); 
    //     if(v[i].z() < zmin) zmin = v[i].z(); 
    //     if(v[i].x() > xmax) xmax = v[i].x(); 
    //     if(v[i].y() > ymax) ymax = v[i].y(); 
    //     if(v[i].z() > zmax) zmax = v[i].z(); 
    // }
    // printf("%lf,%lf,%lf,%lf,%lf,%lf\n",xmin,xmax,ymin,ymax,zmin,zmax);
   
    // Optional: Change this brute force method into a faster one.
    // if(!box_.intersect(r,h,tmin))return false;
    // bool result = false;
    // for (int triId = 0; triId < (int) t.size(); ++triId) {
    //     TriangleIndex& triIndex = t[triId];
    //     Triangle triangle(v[triIndex[0]],
    //                       v[triIndex[1]], v[triIndex[2]], material);
    //     triangle.normal = n[triId];
    //     result |= triangle.intersect(r, h, tmin);
    // }
    // return result;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                t.push_back(trig);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                t.push_back(trig);
            }
        } else if (tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        }
    }
    computeNormal();

    f.close();

    box_ = Box(-0.191055,0.121072,0.067364,0.372852,-0.114066,0.116906,material);
    tree = bvhTree(114514);
    vector<Triangle> Tset;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle.normal = n[triId];
        Tset.push_back(triangle);
    }
    tree.init(Tset,0,0,Tset.size());
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}
