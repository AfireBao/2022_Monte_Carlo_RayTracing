#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <cmath>
#include <iostream>
#include <vecmath.h>

using namespace std;

//implement this class and add more fields as necessary,
class Triangle : public Object3D {

public:
  Triangle() = delete;

  // a b c are three vertex positions of the triangle
  Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m)
      : Object3D(m) {
    this->P0 = a;
	this->P1 = b;
	this->P2 = c;
	this->vertices[0] =a;
	this->vertices[1]=b;
	this->vertices[2]=c;
	normal = Vector3f::cross(a - c,b - c).normalized();
  }

  bool intersect(const Ray &ray, Hit &hit, float tmin) override {
	Vector3f R0 = ray.getOrigin();
	Vector3f Rd = ray.getDirection();
	Vector3f E1 = P0-P1;
	Vector3f E2 = P0-P2;
	Vector3f S = P0-R0;

	Matrix3f M1 = Matrix3f(Rd,E1,E2);
	Matrix3f M2 = Matrix3f(S,E1,E2);
	Matrix3f M3 = Matrix3f(Rd,S,E2);
	Matrix3f M4 = Matrix3f(Rd,E1,S);

    float t = M2.determinant()/M1.determinant();
	float beta = M3.determinant()/M1.determinant();
	float gama = M4.determinant()/M1.determinant();
	//保证交点在三角形内部
	if(t>0 && beta >= 0 && beta <=1 && gama >=0 && gama<=1 && beta + gama <=1){
		if (t > tmin && t < hit.getT()) {
			hit.set(t, material, normal.normalized());
			return true;
			} 
		else {
			return false;
			}
		}
	else{
		return false;
	}
  }
  Vector3f normal;
  Vector3f vertices[3];

protected:
  Vector3f P0;
  Vector3f P1;
  Vector3f P2;
};

#endif // TRIANGLE_H
