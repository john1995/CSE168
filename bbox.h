#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Vector3.h"
#include "Ray.h"
#include <vector>
#include "OpenGL.h"

//Instead of holding pointers to actual primitives, the bounding boxes hold the indices
//of the primitives in the top-level objs* vector passed into BVH::build
class bbox
{

public:
    Vector3   minC;
    Vector3   maxC;
    Object** objects;
    bbox* children[2];  //each box only allowed two children, so can optimize.
    bool isLeaf;
    float distFromRay;
    unsigned int numObjects;
    
    bbox();
    bbox(Vector3, Vector3);
    bbox(Vector3 maxCorner, Vector3 minCorner, unsigned int numObjs);
    bbox(bbox&);     //copy constructor
    ~bbox();
    
    bool intersect(Ray);
    float calcSurfaceArea();
    float calcVolume();
    
    //Tests if box passed in as parameter is intersects this box.
    bool testCollision(bbox*);
    void drawNode();
    
    //level = level of tree. right: left = false, right = true.
    void printBox(int level, bool right);
};




#endif 