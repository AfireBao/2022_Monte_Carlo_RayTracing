#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>


#include "camera.hpp"
#include "group.hpp"
#include "image.hpp"
#include "light.hpp"
#include "scene_parser.hpp"


#include <string>

using namespace std;

#define PI 3.1415926
#define DIFF 0
#define SPEC 1
#define REFR 2
Vector3f radiance(Ray camRay,int dep,unsigned short* X,SceneParser* sceneParser){
  Group *baseGroup = sceneParser->getGroup();
  Hit hit;
  // 判 断camRay是 否 和 场 景 有 交 点， 并 返 回 最 近 交 点 的 数 据， 存
  // 储 在hit中
  // if miss
  if(!baseGroup->intersect(camRay,hit,1e-6)) {
    return Vector3f::ZERO;
    }
  Material* material = hit.getMaterial();
  Vector3f x = camRay.pointAtParameter(hit.getT());
  Vector3f n = hit.getNormal();
  Vector3f f = material->getcolor(x.x(),x.y(),x.z());
  int into = 0;
  Vector3f nl = Vector3f::dot(camRay.getDirection(),n) < 0 ? into = 1, n : -n;
  float P_RR = f.max();
  int refl = material->getType(); 
  // Russion Rolute
  if(++dep>5)
    if(erand48(X) < P_RR) f= f / P_RR;
    else return material->getEnergy();
  if(refl == DIFF){
    Vector3f directLight = Vector3f::ZERO;
    
    // indirect light
    double r1 = 2 * PI * erand48(X);
    double r2 = erand48(X);
    double r2s = sqrt(r2);
    Vector3f w = nl;
    Vector3f u = (Vector3f::cross((fabs(w.x()) > .1 ? Vector3f(0,1,0) : Vector3f(1,0,0)) , w)).normalized();
    Vector3f v = Vector3f::cross(w,u);
    Vector3f d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1-r2)).normalized();
    Vector3f indirectLight =  f * radiance(Ray(x,d),dep,X,sceneParser);
    return material->getEnergy() + indirectLight + directLight; 
  }
  else{
    Ray reflRay = Ray(x,Vector3f::reflect(camRay.getDirection(),n));
    if (refl == SPEC){
      return material->getEnergy() + f * radiance(reflRay,dep,X,sceneParser);
    }
    else{
        Vector3f d = Vector3f::refract(camRay.getDirection(),n,into?1:material->getns(), into?material->getns():1);
        if( d.squaredLength() < 1e-6) // Total internal reflection
          return material->getEnergy() + f * radiance(reflRay,dep,X,sceneParser);
        double a = material->getns() - 1;
        double b = material->getns() + 1;
        double R0 = a*a/(b*b);
        double c = 1. - (into? -Vector3f::dot(camRay.getDirection(),nl) : Vector3f::dot(d,n));
        double Re = R0 + (1.-R0) * c * c * c * c * c;
        double Tr = 1. - Re;
        double P = 0.25 + 0.5 * Re;
        double Rp = Re/P;
        double TP = Tr / (1.-P);
        Vector3f reft_E = material->getEnergy();
        int tmp;
        if(dep > 2){
          if(erand48(X) < P){
            reft_E += f * radiance(reflRay,dep,X,sceneParser) * Rp;
          }
          else{
            Vector3f ra = radiance(Ray(x,d),dep,X,sceneParser);
            // std::cout<<"Tr: "<<Tr<<"Re: "<<Re<<"R0: "<<R0<<"a: "<<a<<"b:"<<b<<"ns:"<<material->getns()<<std::endl;
            // std::cout<<"TP: "<<TP<<"f: "<<f<<"radiance: "<<"x: "<<ra.x()<<"y: "<<ra.y()<<"z: "<<ra.z()<<std::endl;
            reft_E += f * ra * TP;;
          }
        }
        else{
          reft_E += f * radiance(reflRay,dep,X,sceneParser) * Re + radiance(Ray(x,d),dep,X,sceneParser)*Tr;
        }
        // std::cout<<"type: "<<tmp<<"reft_E: "<<"x: "<<reft_E.x()<<"y: "<<reft_E.y()<<"z: "<<reft_E.z()<<std::endl;
        return reft_E;
    }
  }
}

int main(int argc, char *argv[]) {
  for (int argNum = 1; argNum < argc; ++argNum) {
    std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
  }

  if (argc != 3) {
    cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
    return 1;
  }
  string inputFile = argv[1];
  string outputFile = argv[2]; // only bmp is allowed.

  // TODO: Main RayCasting Logic
  // First, parse the scene using SceneParser.
  SceneParser sceneParser = SceneParser(&inputFile[0]);
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
  Camera* camera = sceneParser.getCamera();
  Image img = Image(camera-> getWidth(),camera-> getHeight());
  int sample = 2500;
  // 循 环 屏 幕 空 间 的 像 素
  Vector3f finalColor = Vector3f::ZERO;
  
#pragma omp parallel for schedule(dynamic, 1) private(finalColor)
  for (int x = 0; x<camera-> getWidth(); ++x) {
    fprintf(stderr,"\rUsing %d spp  %5.2f%%",sample*4,100.*x/camera-> getWidth());
    for (int y = 0; y<camera-> getHeight(); ++y) {
      finalColor = Vector3f::ZERO;
      // generate 4 * sample ray from pixel
      for(int sy=0;sy<2;++sy){
        for(int sx=0;sx<2;++sx){
          unsigned short X[3] = {y+sx,y*x+sy,y*x*y+sx*sy};
          for (int ray = 0;ray < sample ;++ray){
            // 计 算 当 前 像 素(x,y)处 相 机 出 射 光 线camRay
            // generate a random number between [0,1]
            double dx = erand48(X);
            double dy = erand48(X);
            Ray camRay = sceneParser.getCamera()-> generateRay(Vector2f(x+dx, y+dy));
            finalColor += radiance(camRay,0,X,&sceneParser);
          }
        }
      }
      finalColor = finalColor / (4 * sample);
      img.SetPixel(x,y,finalColor);
    }
  }
  img.SaveBMP(&outputFile[0]);
  cout << "Hello! Computer Graphics!" << endl;
  return 0;
}
