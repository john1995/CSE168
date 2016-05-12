#include "BVH.h"
#include "Ray.h"
#include "Console.h"

//function for sorting bounding boxes
bool sortObjs(Object* a, Object* b)
{
    //Will be sorted along x-axis position. If same along x, then check y. If same
    //along y, then check z (Hopefully won't have a scene where two triangles are on top
    //of each other).
    Vector3 midpointA((a->boundingBox->minC.x + a->boundingBox->maxC.x) / 2.0f,
                      (a->boundingBox->minC.y + a->boundingBox->maxC.y) / 2.0f,
                      (a->boundingBox->minC.z + a->boundingBox->maxC.z) / 2.0f);
    
    Vector3 midpointB((b->boundingBox->minC.x + b->boundingBox->maxC.x) / 2.0f,
                      (b->boundingBox->minC.y + b->boundingBox->maxC.y) / 2.0f,
                      (b->boundingBox->minC.z + b->boundingBox->maxC.z) / 2.0f);
    
    if (midpointA.x != midpointB.x)
        return midpointA.x < midpointB.x;
    else if (midpointA.y != midpointB.y)
        return midpointA.y < midpointB.y;
    else if (midpointA.z < midpointB.z)
        return midpointA.z < midpointB.z;
    else
        return false;   //They are equal, don't need to be swapped.
}

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;
    
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
            tri->boundingBox = new bbox(maxCorner, minCorner, 1, i);
            
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
            s->boundingBox = new bbox(maxCorner, minCorner, 1, i);
            
            continue;   //go to the next object.
        }
    }
    
    //Now make global box that encompasses all primitives.
    root = new bbox(globalMax, globalMin, objs->size(), 0);
    
    //Figure out where to best split the global box using the surface area heuristic.
    build_recursive(0, objs->size(), root, 0);
}

void BVH::build_recursive(int left_index, int right_index, bbox* box, int depth)
{
    //temporary boxes for calculating cost.
    bbox tempLeft, tempRight;
    
    //holds lowest cost
    float lowestCost = 1000000000000;   //starts at some impossibly high cost.
    int bestSplit = -1;                 //best index to divide primitives at.
    
    //float CostOfRay = (float)m_objects-size() * root.calcSurfaceArea();
    
    //Sort elements in span left_index < - > right_index
    std::sort(m_objects->begin() + left_index, m_objects->begin() + right_index, sortObjs);
    
    //Check multiple potential split planes parallel to the yz-plane and
    //perpendicular to the xz-plane
    //temp. variables for storing potential dimensions of child boxes
    Vector3 lMin, lMax, rMin, rMax;
    
    //Initial left bounding box just includes object at left_index.
    lMin = m_objects->at(left_index)->boundingBox->minC;
    lMax = m_objects->at(left_index)->boundingBox->maxC;
    
    rMin.set(100000.0f, 100000.0f, 100000.0f);    //easily find smaller corner
    rMax.set(-100000.0f, -100000.0f, -100000.0f);     //easily find larger corner
    
    //Go through whole list between left and right index and find a split index that has a
    //cost less than the cost of intersecting with all the intersectables
    //spliti is the spliting index
    for(int spliti = left_index+1; spliti < right_index; spliti++)
    {
        //go through the left split and find the min/max for the leftbox
        //the Lmin.x is always the left most box
        lMin.x = m_objects->at(left_index)->boundingBox->minC.x;
        
        //min between current y min. and y of new triangle
        lMin.y = std::min(lMin.y, m_objects->at(spliti - 1)->boundingBox->minC.y);
        
        //min between current z min. and z of new triangle
        lMin.z = std::min(lMin.z, m_objects->at(spliti - 1)->boundingBox->minC.z);
        
        //the Lmax.x is always the right most box
        lMax.x = m_objects->at(spliti - 1)->boundingBox->maxC.x;
        
        //max between current y max and y coord. of maxCorner of bbox of new tri
        lMax.y = std::max(lMax.y, m_objects->at(spliti - 1)->boundingBox->maxC.y);
        
        //max between current z max and the z coord. of maxCorner of bbox of new tri
        lMax.z = std::max(lMax.z, m_objects->at(spliti - 1)->boundingBox->maxC.z);
        
        //ASK IF THERE IS BETTER WAY TO FIND DIMENSIONS OF RIGHT BB.
        //WOULD BE NICE TO ELIMINATE LOOP.
        
        //Can isolate x dimensions of right box
        rMin.x = m_objects->at(spliti)->boundingBox->minC.x;
        rMax.x = m_objects->at(right_index-1)->boundingBox->maxC.x;
        
        //Calculate y and z dimensions of the right box
        for(int k = spliti; k < right_index; k++)
        {
            rMin.y = std::min(rMin.y, m_objects->at(k)->boundingBox->minC.y);
            rMin.z = std::min(rMin.z, m_objects->at(k)->boundingBox->minC.z);
            
            rMax.y = std::max(rMax.y, m_objects->at(k)->boundingBox->maxC.y);
            rMax.z = std::max(rMax.z, m_objects->at(k)->boundingBox->maxC.z);
        }
        
        tempLeft.minC.set(lMin);
        tempLeft.maxC.set(lMax);
        tempLeft.index = left_index;
        tempLeft.numObjects = spliti - left_index;
        
        tempRight.minC.set(rMin);
        tempRight.maxC.set(rMax);
        tempRight.index = spliti;
        tempRight.numObjects = right_index - spliti;
        
        //Compute cost of split
        float c = (tempLeft.calcSurfaceArea()/box->calcSurfaceArea()) *
        (float)tempLeft.numObjects + (tempRight.calcSurfaceArea()/box->calcSurfaceArea()) *
        (float)tempRight.numObjects;
        
        if( c < lowestCost)
        {
            lowestCost = c;
            bestSplit = spliti;
            
            //delete old left child and right child
            delete box->children[0];
            delete box->children[1];
            
            //create new best potential candidate bounding boxes
            box->children[0] = new bbox(tempLeft);
            box->children[1] = new bbox(tempRight);
        }
    }
    //End of loop. Should have optimal place to split at. Check if its cost
    //is lower than just raytracing from parent
    float nc = box->numObjects; //Not sure if this is right.
    
    if (nc < lowestCost)
    {
        //delete the child boxes, initialize node as a leaf
        delete box->children[0];
        delete box->children[1];
        box->isLeaf = true;
    }
    else
    {
        //Recursively split.
        build_recursive(left_index, bestSplit, box->children[0], depth + 1);
        build_recursive(bestSplit, right_index, box->children[1], depth + 1);
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