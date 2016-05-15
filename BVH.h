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

class BVH
{
public:
    BVH();
    void build(Objects * objs);
    void build_recursive(int left_index, int right_index, bbox* box, int depth);

    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
    Objects * m_objects;
    
    bbox* root;
    const uint MAX_BINS;   //number of bins to place primitives into at each level.
};

#endif // CSE168_BVH_H_INCLUDED
