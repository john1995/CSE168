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
    
    HitInfo childHit;
    float minDistance = 1000000000000.0f;   //Some impossibly large number.
    bbox* closestBox = nullptr;
    
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
            if ((*it)->intersect(ray, childHit))
            {
                //Save this
                if (childHit.t < minDistance)
                {
                    minDistance = childHit.t;
                    closestBox = *it;
                }
            }
        }
        
        if (closestBox != nullptr)
        {
            //Recursively call intersect on next lowest box
            return intersect(closestBox, minHit, ray);
        }
        else
        {
            //No intersections with child boxes.
            return false;
        }
    }
        
}
