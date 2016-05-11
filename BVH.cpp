#include "BVH.h"
#include "Ray.h"
#include "Console.h"

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;
}


bool
BVH::intersect(bbox *box, HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    /*bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    //Loop through all objects to check if ray intersects any one of them.
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
        if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
        {
            hit = true;
            //Only update hitinfo if object we hit is closer to camera
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
    }
    
    return hit;*/
    
    if (box->isLeaf)
    {
        //Check if ray hits any of the triangles in the leaf
        return false;   //stub for now.
    }
    else
    {
        //Check all child boxes.
        for (std::vector<bbox*>::iterator it = box->children.begin();
             it != box->children.end(); it++)
        {
            if ((*it)->intersect(ray))
            {
                //Go down the hierarchy
                return intersect(*it, minHit, ray);
            }
        }
        
        //If reach here, it means we went through all children
        //and didn't find a single intersection.
        return false;
        
    }
        
}
