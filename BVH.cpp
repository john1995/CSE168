#include "BVH.h"
#include "Ray.h"
#include "Console.h"

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;
    
    bbox* primitiveBoxes[objs->size()];
    
    //Vectors to store farthest reaching positions of any primitive.
    //Useful for constructing global bounding box.
    Vector3 globalMax, globalMin;
    
    //Place a bounding box around every primitive initially.
    //Triangles and spheres should be in objects vector,
    //so just loop through that.
    for (int i = 0; i < objs->size(); i++)
    {
        //See if we can cast to a triangle.
        Triangle* tri = dynamic_cast<Triangle*>(objs->at(i));
        
        if (tri != NULL)
        {
            //Successfully casted to a triangle. Now get its
            //vertices.
            TriangleMesh::TupleI3 ti3 = tri->getMesh()->vIndices()[tri->getIndex()];
            
            //Get each of its vertices through their indices in the mesh, which
            //are stored in a custom-defined 'tuple' within the triangle mesh
            const Vector3 & v0 = tri->getMesh()->vertices()[ti3.x]; //vertex a of triangle
            const Vector3 & v1 = tri->getMesh()->vertices()[ti3.y]; //vertex b of triangle
            const Vector3 & v2 = tri->getMesh()->vertices()[ti3.z]; //vertex c of triangle
            
            //find min_x, min_y, min_z, max_x, max_y, max_z so know
            //where to place boundaries of box.
            float min_x = std::min(v0.x, std::min(v1.x, v2.x));
            float min_y = std::min(v0.y, std::min(v1.y, v2.y));
            float min_z = std::min(v0.z, std::min(v1.z, v2.z));
            
            float max_x = std::max(v0.x, std::max(v1.x, v2.x));
            float max_y = std::max(v0.y, std::max(v1.y, v2.y));
            float max_z = std::max(v0.z, std::max(v1.z, v2.z));
            
            Vector3 maxCorner(max_x, max_y, max_z);
            Vector3 minCorner(min_x, min_y, min_z);
            
            //Check if global min and max need to be updated.
            globalMin.x = std::min(globalMin.x, minCorner.x);
            globalMin.y = std::min(globalMin.y, minCorner.y);
            globalMin.z = std::min(globalMin.z, minCorner.z);
            globalMax.x = std::max(globalMax.x, maxCorner.x);
            globalMax.y = std::max(globalMax.y, maxCorner.y);
            globalMax.z = std::max(globalMax.z, maxCorner.z);
            
            //construct a new bounding box, push it back into array of primitive boxes.
            primitiveBoxes[i] = new bbox(maxCorner, minCorner, 1, i);
            
            continue;   //Go onto the next object.
        }
        
        //Try Sphere primitive now
        Sphere* s = dynamic_cast<Sphere*>(objs->at(i));
        
        if (s != NULL)
        {
            //Successfully casted to a sphere. Subtract radius from center to get min corner,
            //add radius to center to get max corner.
            Vector3 minCorner = s->center() - s->radius();
            Vector3 maxCorner = s->center() + s->radius();
            
            //Check if global min and max need to be updated.
            globalMin.x = std::min(globalMin.x, minCorner.x);
            globalMin.y = std::min(globalMin.y, minCorner.y);
            globalMin.z = std::min(globalMin.z, minCorner.z);
            globalMax.x = std::max(globalMax.x, maxCorner.x);
            globalMax.y = std::max(globalMax.y, maxCorner.y);
            globalMax.z = std::max(globalMax.z, maxCorner.z);
            
            //construct new bounding box, push it back into array of primitive boxes.
            primitiveBoxes[i] = new bbox(maxCorner, minCorner, 1, i);
            
            continue;   //go to the next object.
        }
    }
    
    //Now make global box that encompasses all primitives.
    root = new bbox(globalMax, globalMin, objs->size(), 0);
    
    //Figure out where to best split the global box using the surface area heuristic.
    //build_recursive(int 0, int objs->size(), root, 0);
}

void BVH::build_recursive(int left_index, int right_index, bbox* box, int depth)
{
    
}


bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    bbox* currentNode = root;
    
    //vector being used as a stack to hold nodes that may need to be checked later.
    std::vector<bbox*> nodeStack;
    
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    //intersection test with root
    if (!currentNode->intersect(ray))
        return false;
    
    HitInfo lChildHitInfo, rChildHitInfo;
    
    //loop will terminate if we hit primitive or if we miss bounding box/primitive.
    while (1)
    {
        //if not a leaf, check for intersection with both child boxes
        if (!currentNode->isLeaf)
        {
            //case: both child bounding boxes are intersected.
            if (currentNode->children[0]->intersect(ray) &&
                currentNode->children[1]->intersect(ray))
            {
                //One farther away gets pushed onto stack and one
                //closer is the one we will check next.
                if (currentNode->children[0]->distFromRay >=
                    currentNode->children[1]->distFromRay)
                {
                    nodeStack.push_back(currentNode->children[0]);
                    currentNode = currentNode->children[1];
                }
                else
                {
                    nodeStack.push_back(currentNode->children[1]);
                    currentNode = currentNode->children[0];
                }
            }
            //case: only right child is intersected
            else if (!currentNode->children[0]->intersect(ray) &&
               currentNode->children[1]->intersect(ray))
            {
                currentNode = currentNode->children[1];
            }
            //case: only left child is intersected
            else if (currentNode->children[0]->intersect(ray) &&
                     !currentNode->children[1]->intersect(ray))
            {
                currentNode = currentNode->children[0];
            }
            //case: neither children node intersected
            else { /* Do nothing */ }
        }
        else
        {
            //We have reached a leaf. Check primitives inside.
            for (int i = currentNode->index; i < currentNode->index + currentNode->numObjects; i++)
            {
                if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
                {
                    hit = true;
                    //Only update hitinfo if object we hit is closer to camera
                    if (tempMinHit.t < minHit.t)
                        minHit = tempMinHit;
                }
            }
            
            if (hit)
            {
                //we hit something and know its the closest primitive in the box,
                //so return true
                return true;
            }
        }
        
        //Pop nodes off stack, see if any have t < minHit.t
        if (!nodeStack.empty())
        {
            currentNode = nodeStack.back();
            nodeStack.pop_back();
        }
        
        while (currentNode->distFromRay > minHit.t && !nodeStack.empty())
        {
            currentNode = nodeStack.back();
            nodeStack.pop_back();
        }
        
        //If we make it here, we haven't already returned from a hit, and all
        //child nodes that were farther away have been tested. Therefore, there
        //was no hit, and we can return false.
        if (nodeStack.empty())
            return false;
    }
}
