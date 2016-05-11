#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "bbox.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "Sphere.h"
#include <algorithm> 
#include "Vector3.h"

class BVH
{
public:
    void build(Objects * objs);
    void build_recursive(int left_index, int right_index, bbox* box, int depth);

    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
    Objects * m_objects;
    bbox* root;
};

//struct for sorting bounding boxes
struct sortObj
{
    bool operator() (bbox* a, bbox* b)
    {
        //Will be sorted along x-axis position. If same along x, then check y. If same
        //along y, then check z (Hopefully won't have a scene where two triangles are on top
        //of each other).
        Vector3 midpointA((a->minC.x + a->maxC.x) / 2.0f,
                          (a->minC.y + a->maxC.y) / 2.0f
                          (a->minC.z + a->maxC.z) / 2.0f);
        
        Vector3 midpointB(((b->minC.x + b->maxC.x) / 2.0f,
                           (b->minC.y + b->maxC.y) / 2.0f
                           (b->minC.z + b->maxC.z) / 2.0f));
        
        if (midpointA.x != midpointB.x)
            return midpointA.x < midpointB.x;
        else if (midpointA.y != midpointB.y)
            return midpointA.y < midpointB.y;
        else if (midpointA.z < midpointB.z)
            return midpointA.z < midpointB.z;
        else
            return false;   //They are equal, don't need to be swapped.
    }
} sorter;

#endif // CSE168_BVH_H_INCLUDED
