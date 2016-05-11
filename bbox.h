#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Vector3.h"
#include "Ray.h"
#include "Object.h"
#include <vector>

class bbox
{

public:
    bbox();
    bbox(Vector3, Vector3);
    ~bbox();
    Vector3   minC;
    Vector3   maxC;
    Object *obj;
    bool intersect(Ray, HitInfo& h);
    std::vector<bbox*> children;
    bool isLeaf;

};




#endif 