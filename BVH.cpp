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
            
            //keep an index to that box in the array
            objs->at(i).index = i;
            
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
            //keep an index to that box in the array
            objs->at(i).index = i;
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
    Vector3 Lmax, Rmax = new Vector3(1000000.0f,1000000.0f,1000000.0f);
    Vector3 Lmin, Rmin = new Vector3(-1000000.f,-1000000.0f,-1000000.0f);

    bbox Lchild,Rchild;
    
    //float CostOfRay = (float)m_objects-size() * root.calcSurfaceArea();
    
    //Sort elements in span left_index < - > right_index
    std::sort(m_objects->begin() + left_index, m_objects->begin() + right_index, sorter);
    
    //Check multiple potential split planes parallel to the yz-plane and
    //perpendicular to the xz-plane
    
    //Go through whole list between left and right index and find a split index that has a
    //cost less than the cost of intersecting with all the intersectables
    //spliti is the spliting index
    for(int spliti = left_index+1; spliti < right_index; spliti++)
    {
        //Calculate left box
        for(int j = left_index; j < spliti; j++)
        {
            //objects now have and index field used to find its box in the boxarray
            bbox temp = primitiveBoxes[m_objects->at(j).index];
           
            
            //go through the left split and find the min/max for the leftbox
            //the Lmin.x is always the left most box
            Lmin.x = primitiveBoxes[m_objects->at(left_index).index].x;
            Lmin.y = std::min(Lmin.y, temp.minC.y);
            Lmin.z = std::min(Lmin.z, temp.minC.z);
            
            //the Lmax.x is always the right most box
            Lmax.x = primitiveBoxes[m_objects->at(spliti-1).index].x;
            Lmax.y = std::max(Lmax.y, temp.maxC.y);
            Lmax.z = std::max(Lmax.z, temp.maxC.z);

        }
        
        
        Lchild = new bbox(Lmax,Lmin, spliti - leftindex, 0);
        
        //Calculate right box
        for(int k = spliti; k < right_index; k++)
        {
            //same logic as left box
            bbox temp = primitiveBoxes[m_objects->at(k).index];
            
            Rmin.x = primitiveBoxes[m_objects->at(spliti).index].x;
            Rmin.y = std::min(Rmin.y, temp.minC.y);
            Rmin.z = std::min(Rmin.z, temp.minC.z);
            
            Rmax.x = primitiveBoxes[m_objects->at(right_index-1).index].x;
            Rmax.y = std::max(Rmax.y, temp.maxC.y);
            Rmax.z = std::max(Rmax.z, temp.maxC.z);

        }
        
        Rchild = new bbox(Rmax,Rmin, right_index - spliti, 0);
        
        //Compute cost of split
        float c = (Lchild.calcSurfaceArea()/box.calcSurfaceArea()) * (float)Lchild.numObjects +
                  (Rchild.calcSurfaceArea()/box.calcSurfaceArea()) * (float)Rchild.numObjects;
        
        //Computer cost of all intersectables
        float nc = box.numObjects; //Not sure if this is right.
        
        if( c < nc)
        {
            //assign children
            box->children[0] = Lchild;
            box->children[1] = Rchild;
            
            //Use this split for the recursion

            build_recursive(left_index, spliti, box->children[0], depth + 1)
            build_recursive(spliti, right_index, box->children[1], depth + 1)
        }
        else
        {
          //dont split at this index. This is a leaf node. Not sure what to do here.
        }
    }
    
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
                    continue;
                }
                else
                {
                    nodeStack.push_back(currentNode->children[1]);
                    currentNode = currentNode->children[0];
                    continue;
                }
            }
            //case: only right child is intersected
            else if (!currentNode->children[0]->intersect(ray) &&
               currentNode->children[1]->intersect(ray))
            {
                currentNode = currentNode->children[1];
                continue;
            }
            //case: only left child is intersected
            else if (currentNode->children[0]->intersect(ray) &&
                     !currentNode->children[1]->intersect(ray))
            {
                currentNode = currentNode->children[0];
                continue;
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
