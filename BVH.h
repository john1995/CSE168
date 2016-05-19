#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "bbox.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "Sphere.h"
#include <algorithm>
#include <climits>
#include "Vector3.h"
#include "Plane.hpp"

class BVH
{
public:
    BVH();
    void build(Objects * objs);
    void splitNode(bbox* box, int depth);

    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;
    bool intersectNode(bbox* box, HitInfo& minHit, const Ray& ray, float t_min, float t_max,
                       int depth) const;
    
    float calcSurfaceArea(Vector3 minC, Vector3 maxC);
    
    void printBVH();
    
    void printStatistics();
    
    void drawBVH();
    
    static uint numNodes, numLeaves, rayBoxIntersections, rayTriIntersections;

protected:
    Objects * m_objects;
    
    bbox* root;
    const uint MAX_BINS;   //number of bins to place primitives into at each level.
};

#endif // CSE168_BVH_H_INCLUDED
