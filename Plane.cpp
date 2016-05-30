//
//  Plane.cpp
//  
//
//  Created by Macklin Groot on 5/17/16.
//
//

#include "Plane.hpp"

bool Plane::intersect(HitInfo& result, const Ray& ray,
                 float tMin, float tMax)
{
    float angle = dot(normal, ray.d);
    if (angle == 0)
    {
        //printf("Ray is parallel to the plane\n");
        return false;   //parallel to plane
    }
    
    result.t = dot(normal, (point - ray.o)) / angle;
    result.P = ray.o + result.t * ray.d;
    result.N = normal;
    
    if (result.t > tMin && result.t < tMax)
        return true;
    else
        return false;
}