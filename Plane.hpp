//
//  Plane.hpp
//  
//
//  Created by Macklin Groot on 5/17/16.
//
//

#ifndef Plane_hpp
#define Plane_hpp

#include <stdio.h>
#include "Object.h"
#include "Vector3.h"
#include "Ray.h"

class Plane : public Object
{
public:
    Vector3 normal;
    Vector3 point;
    
    Plane(Vector3 n, Vector3 p) : normal(n), point(p) {}
    
    virtual bool intersect(HitInfo& result, const Ray& ray,
                           float tMin = 0.0f, float tMax = MIRO_TMAX);
};

#endif /* Plane_hpp */
