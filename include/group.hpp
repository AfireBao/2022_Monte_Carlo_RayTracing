#ifndef GROUP_H
#define GROUP_H

#include "hit.hpp"
#include "object3d.hpp"
#include "ray.hpp"
#include <iostream>
#include <vector>

// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:
  Group() {}

  explicit Group(int num_objects) { object.resize(num_objects); }

  ~Group() override {}

  bool intersect(const Ray &r, Hit &h, float tmin) override {
    bool isintersect = false;
    for (int i = 0; i < object.size(); i++) {
      if (object[i]->intersect(r, h, tmin)) {
        isintersect = true;
      }
    }
    if (isintersect) {
      return true;
    } else {

      return false;
    }
  }
  bool intersectFrom(const Ray &r, Hit &h, float tmin, int start){
    bool isintersect = false;
    for (int i = start; i < object.size(); i++) {
      if (object[i]->intersect(r, h, tmin)) {
        isintersect = true;
      }
    }
    if (isintersect) {
      return true;
    } else {

      return false;
    }
  }

  void addObject(int index, Object3D *obj) { object[index] = obj; }

  int getGroupSize() { return object.size(); }

  Object3D* getObj(int index){
    if(index < object.size()){
      return object[index];
    }
  }

private:
  std::vector<Object3D *> object;
};

#endif
