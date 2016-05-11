#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Vector3.h"
#include "Ray.h"
#include "Object.h"
#include <vector>

//Instead of holding pointers to actual primitives, the bounding boxes hold the indices
//of the primitives in the top-level objs* vector passed into BVH::build
class bbox
{

public:
    Vector3   minC;
    Vector3   maxC;
    unsigned int numObjects;
    unsigned int index; // if not leaf, index of left child. If leaf, index of 1st primitive.
    bbox* children[2];  //each box only allowed two children, so can optimize.
    bool isLeaf;
    int distFromRay;
    
    bbox();
    bbox(Vector3, Vector3);
    bbox (Vector3 maxCorner, Vector3 minCorner, unsigned int numObjs, unsigned int index);
    ~bbox();
    
    bool intersect(Ray);
    float calcSurfaceArea();
};




#endif 