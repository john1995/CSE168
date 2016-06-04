#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include <math.h>

#undef near
#undef far

unsigned int BVH::numNodes = 0;
unsigned int BVH::numLeaves = 0;
unsigned int BVH::rayBoxIntersections = 0;
unsigned int BVH::rayTriIntersections = 0;

BVH::BVH() : MAX_BINS(10) {}

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
    
    root = new bbox(Vector3(0.0f, 0.0f, 0.0f),
                    Vector3(0.0f, 0.0f, 0.0f),
                    objs->size());
    
    //Place a bounding box around every primitive initially.
    //Triangles and spheres should be in objects vector,
    //so just loop through that.
    for (int i = 0; i < objs->size(); i++)
    {
        //See if we can cast to a triangle.
        Triangle* tri = dynamic_cast<Triangle*>(objs->at(i));
        
        if (tri != NULL)
        {
            root->objects[i] = tri;
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
            tri->boundingBox = new bbox(maxCorner, minCorner, 1);
            tri->centroid = (maxCorner + minCorner) / 2.0f;
            
            continue;   //Go onto the next object.
        }
        
        //Try Sphere primitive now
        Sphere* s = dynamic_cast<Sphere*>(objs->at(i));
        
        if (s != NULL)
        {
            root->objects[i] = s;
            
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
            s->boundingBox = new bbox(maxCorner, minCorner, 1);
            s->centroid = s->center();
            
            continue;   //go to the next object.
        }
    }
    
    //printf("minCorner of global box: %f %f %f\n", globalMin.x, globalMin.y, globalMin.z);
    //printf("maxCorner of global box: %f %f %f\n", globalMax.x, globalMax.y, globalMax.z);
    
    //Set boundaries for root.
    root->minC = globalMin;
    root->maxC = globalMax;

    numNodes++;
    //printf("number of objects in the root: %u\n", root->numObjects);
    
    //Figure out where to best split the global box using the surface area heuristic.
    splitNode(root, 0);
}

void BVH::splitNode(bbox* box, int depth)
{
    box->left = nullptr;
    box->right = nullptr;
    
    //printf("In build_recursive\n");
    //printf("Depth: %u\n", depth);
    //printf("Volume of box to potentially split: %f\n", box->calcVolume());
    //Stop when number of objects in box <= 8
    if (box->numObjects <= 8 || depth > 200)
    {
        //printf("Stopping recursion here: num objects in leaf: %u\n", box->numObjects);
        numLeaves++;
        //this is a leaf. Store index of first object, amount in
        box->isLeaf = true;
        //printf("8 or fewer objects in box, returning\n");
        
        //printf("At leaf, depth = %u\n", depth);
        
        return;
    }
    
    bbox* tempL = new bbox();
    bbox* tempR = new bbox();
    bbox* bestL = new bbox();
    bbox* bestR = new bbox();
    float bestPlanePos = 0.0f;
    
    Plane *splitPlane;
    tempL->minC = box->minC;
    tempL->maxC = box->maxC;
    tempR->minC = box->minC;
    tempR->maxC = box->maxC;
    
    
    //calculate the largest dimension of this bounding box. Will determine which
    //dimensions we will be dividing bins on. Got to be better way to do this.
    float xLength = box->maxC.x - box->minC.x;
    float yLength = box->maxC.y - box->minC.y;
    float zLength = box->maxC.z - box->minC.z;
    
    if (xLength > yLength && xLength > zLength)
    {
        box->axis = 0;
        splitPlane = new Plane(Vector3(1.0f, 0.0f, 0.0f),
                               Vector3(0.0f, 0.0f, 0.0f));
    }
    else if (yLength > xLength && yLength > zLength)
    {
        box->axis = 1;
        splitPlane = new Plane(Vector3(0.0f, 1.0f, 0.0f),
                               Vector3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        box->axis = 2;
        splitPlane = new Plane(Vector3(0.0f, 0.0f, 1.0f),
                               Vector3(0.0f, 0.0f, 0.0f));
    }
    
    /**********************************************************************/
    
    //holds lowest cost
    float lowestCost = -1;  //starts at negative cost.
    
    //flags for determining if primitives in this box intersect children. Useful so
    //don't have to call testCollision multiple times.
    bool intersectLeft, intersectRight;
    
    float c = 0.0f;    //where lowest cost will be stored
    
    float saLeft, saRight, saParent;
    
    //Go through whole list of bins and find optimal place to split. Only consider longest axis
    for(int i = 1; i < MAX_BINS; ++i)
    {
        //First need to adjust min corners and max corners of left and right boxes.
        if (box->axis == 0) //longest dimension is x
        {
            //find length of potential box in x direction
            float xCoord = (xLength / MAX_BINS) * i + box->minC.x;
            box->plane_pos = xCoord;
            splitPlane->point = Vector3(xCoord, 0.0f, 0.0f);
            tempL->maxC.x = xCoord;
            tempR->minC.x = xCoord;
        }
        else if (box->axis == 1)   //longest dimension is y
        {
            //find length of potential box in y direction
            float yCoord = (yLength / MAX_BINS) * i + box->minC.y;
            box->plane_pos = yCoord;
            splitPlane->point = Vector3(0.0f, yCoord, 0.0f);
            tempL->maxC.y = yCoord;
            tempR->minC.y = yCoord;
        }
        else    //longest dimension is z
        {
            //find length of potential box in z direction
            float zCoord = (zLength / MAX_BINS) * i + box->minC.z;
            box->plane_pos = zCoord;
            splitPlane->point = Vector3(0.0f, 0.0f, zCoord);
            tempL->maxC.z = zCoord;
            tempR->minC.z = zCoord;
        }
        
        //determine if the objects in this box intersect with the left child and/or the right
        //child.
        for (int i = 0; i < box->numObjects; i++)
        {
            intersectLeft = box->objects[i]->boundingBox->testCollision(tempL);
            intersectRight = box->objects[i]->boundingBox->testCollision(tempR);
            
            if (intersectLeft && intersectRight)
            {
                tempL->numObjects++;
                tempR->numObjects++;
            }
            else if (intersectLeft)
            {
                tempL->numObjects++;
            }
            else if (intersectRight)
            {
                tempR->numObjects++;
            }
        }
        //printf("num objects in left: %u\n", tempL->numObjects);
        //printf("num objects in right: %u\n", tempR->numObjects);
        
        saLeft = tempL->calcSurfaceArea();
        saRight = tempR->calcSurfaceArea();
        saParent = box->calcSurfaceArea();
    
        c = (saLeft / saParent) * tempL->numObjects + (saRight / saParent) * tempR->numObjects;
        
        //update children and cost if lower than current lowest.
        if( c < lowestCost || lowestCost == -1)
        {
            lowestCost = c;
            bestPlanePos = box->plane_pos;
            
            bestL->copy(tempL);
            //printf("bestL numObjects: %u\n", bestL->numObjects);
            bestR->copy(tempR);
            //printf("bestR numObjects: %u\n", bestR->numObjects);
        }
    }
    
    //Check if split is worth it.
    if (lowestCost >= box->numObjects)
    {
        //printf("Cost of splitting is greater than cost of intersecting here. "
        //       "Num triangles in leaf: %u\n", box->numObjects);
        numLeaves++;
        //this is a leaf. Store index of first object, amount in
        box->isLeaf = true;
        
        //printf("At leaf, depth = %u\n", depth);
        
        //delete left child and right child. Should be a leaf.
        box->left = nullptr;
        box->right = nullptr;
        
        return;
    }
    
    //At this point, have found best split, and know splitting is worth it. Create child boxes.
    box->left = new bbox(*bestL);
    box->right = new bbox(*bestR);
    
    //printf("box->left numObjects: %u\n", box->left->numObjects);
    //printf("box->right numObjects: %u\n", box->right->numObjects);
    box->plane_pos = bestPlanePos;
    
    int lCounter = 0;
    int rCounter = 0;
    //loop through every object in box we are splitting, see which child it falls in to
    for (int i = 0; i < box->numObjects; i++)
    {
        if (box->left->testCollision(box->objects[i]->boundingBox))
        {
            box->left->objects[lCounter++] = box->objects[i];
        }
        
        if (box->right->testCollision(box->objects[i]->boundingBox))
        {
            box->right->objects[rCounter++] = box->objects[i];
        }
    }
    
    //Recursively split.
    numNodes++;
    splitNode(box->left, depth + 1);
    
    //delete objects in nodes that aren't leaves - only need to know about their
    //children, not the objects inside of them.
    if (!box->left->isLeaf)
    {
        delete[] box->left->objects;
    }
    
    numNodes++;
    splitNode(box->right, depth + 1);
    
    if (!box->right->isLeaf)
    {
        delete[] box->right->objects;
    }
    
    delete splitPlane;
    delete tempL;
    delete tempR;
    delete bestL;
    delete bestR;
}

bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    
    /*// Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.
    
    bool hit = false;
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
    
    bbox* currentNode = root;

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    //intersection test with root
    if (!currentNode->intersect(ray, tMin, tMax))
        return false;
    else
        return intersectNode(root, minHit, ray, tMin, tMax, 0);
    
}

bool BVH::intersectNode(bbox* box, HitInfo& minHit, const Ray& ray, float t_min, float t_max,
                        int depth) const
{
    bbox *near, *far;
    Plane* splitPlane;
    HitInfo planeInfo;
    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = t_max;
    //printf("top of intersectNode, level %u\n", depth);
    //printf("Plane position of box: %f\n", box->plane_pos);
    
    if (box->axis == 0)
        splitPlane = new Plane(Vector3(1.0f, 0.0f, 0.0f),
                               Vector3(box->plane_pos, 0.0f, 0.0f));
    else if (box->axis == 1)
        splitPlane = new Plane(Vector3(0.0f, 1.0f, 0.0f),
                               Vector3(0.0f, box->plane_pos, 0.0f));
    else
        splitPlane = new Plane(Vector3(0.0f, 0.0f, 1.0f),
                               Vector3(0.0f, 0.0f, box->plane_pos));
    
    //if not a leaf, check for intersection with both child boxes
    if (!box->isLeaf)
    {
        //printf("Box is not a leaf\n");
        if (ray.d[box->axis] > 0)
        {
            near = box->left;
            far = box->right;
        }
        else
        {
            near = box->right;
            far = box->left;
        }
        
        splitPlane->intersect(planeInfo, ray);
		delete splitPlane;
        if (planeInfo.t > t_max)    //only intersects left box
        {
            rayBoxIntersections++;
            return intersectNode(near, minHit, ray, t_min, t_max, depth + 1);
        }
        else if (planeInfo.t < t_min)   //only intersects right box
        {
            rayBoxIntersections++;
            return intersectNode(far, minHit, ray, t_min, t_max, depth + 1);
        }
        else
        {
            rayBoxIntersections += 2;
            if (intersectNode(near, minHit, ray, t_min, planeInfo.t, depth + 1))
                return true;
            else
                return intersectNode(far, minHit, ray, planeInfo.t, t_max, depth + 1);
        }
    }
    else
    {
        //printf("intersected a leaf! Level: %u, numObjects in leaf: %u\n", depth, box->numObjects);
        depth--;
        
        //We have reached a leaf. Check primitives inside.
        for (int i = 0; i < box->numObjects; i++)
        {
            if (box->objects[i]->intersect(tempMinHit, ray, t_min, t_max))
            {
                rayTriIntersections++;
                hit = true;
                //Only update hitinfo if object we hit is closer to camera
                if (tempMinHit.t < minHit.t)
                {
                    minHit = tempMinHit;
                
                    //printf("Hit an object!\n");
                }
            }
        }
        
        
        if (hit && minHit.t < t_max)
        {
            //we hit something and know its the closest primitive in the box,
            //so return true
            //printf("returning true\n");
            return true;
        }
        else
            return false;
    }
}

float BVH::calcSurfaceArea(Vector3 minC, Vector3 maxC)
{
    float height = maxC.y - minC.y;
    float width = maxC.x - minC.x;
    float length = maxC.z - minC.z;
    
    return 2 * (height * width) + 2 * (height * length) + 2 * (width * length);
}

void BVH::printBVH()
{
    root->printBox(1, false);
}

void BVH::printStatistics()
{
    printf("BVH Statistics: \n\n");
    printf("Number of nodes: %u\n", numNodes);
    printf("Number of leaves: %u\n", numLeaves);
    printf("Number of ray-box intersections: %u\n", rayBoxIntersections);
    printf("Number of ray-triangle intersections: %u\n\n", rayTriIntersections);
}

void BVH::drawBVH()
{
    root->drawNode();
}
