#include "bbox.h"
#include <algorithm> 

bbox::bbox()
{
    
}

bbox::bbox(Vector3 max, Vector3 min)
{
    maxC = max;
    minC = min;
}

bbox::bbox(Vector3 maxCorner, Vector3 minCorner, unsigned int numObjs, unsigned int index) :
maxC(maxCorner), minC(minCorner), numObjects(numObjs), index(index) {}

bbox::bbox(bbox& copy) : maxC(copy.maxC), minC(copy.minC), numObjects(copy.numObjects),
index(copy.index) {}

bbox::~bbox()
{
}

bool bbox::intersect(Ray r)
{
    Vector3 t1;
    Vector3 t2;
    Vector3 tmin;
    Vector3 tmax;
    
    t1.x = (minC.x - r.o.x) / r.d.x;
    t1.y = (minC.y - r.o.y) / r.d.y;
    t1.z = (minC.z - r.o.z) / r.d.z;

    t2.x = (maxC.x - r.o.x) / r.d.x;
    t2.y = (maxC.y - r.o.y) / r.d.y;
    t2.z = (maxC.z - r.o.z) / r.d.z;
    
    
    tmax.x = std::max(t1.x, t2.x);
    tmax.y = std::max(t1.y, t2.y);
    tmax.z = std::max(t1.z, t2.z);
    
    tmin.x = std::min(t1.x, t2.x);
    tmin.y = std::min(t1.y, t2.y);
    tmin.z = std::min(t1.z, t2.z);
    
    float t_min = std::max(tmin.x, std::max(tmin.y, tmin.z));
    float t_max = std::min(tmax.x, std::max(tmax.y, tmax.z));
    
    if (t_min < t_max)
    {
        //Distance to closest plane we hit.
        distFromRay = std::min(tmin.x, std::min(tmin.y, tmin.z));
        return true;
    }
    else
    {
        return false;
    }
}

float bbox::calcSurfaceArea()
{
    float height = maxC.y - minC.y;
    float width = maxC.x - minC.x;
    float length = maxC.z - minC.z;
    
    return 2 * (height * width) + 2 * (height * length) + 2 * (width * length);
}