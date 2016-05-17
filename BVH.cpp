#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include <math.h>

uint BVH::numNodes = 0;
uint BVH::numLeaves = 0;
uint BVH::rayBoxIntersections = 0;
uint BVH::rayTriIntersections = 0;

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
    
    unsigned int objsInLeft, objsInRight;
    Vector3 toSplitPlane;
    Plane *splitPlane;
    
    
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
            splitPlane->point = Vector3(xCoord, box->maxC.y, box->maxC.z);
            toSplitPlane = Vector3(xCoord - box->minC.x, 0.0f, 0.0f);
        }
        else if (box->axis == 1)   //longest dimension is y
        {
            //find length of potential box in y direction
            float yCoord = (yLength / MAX_BINS) * i + box->minC.y;
            box->plane_pos = yCoord;
            splitPlane->point = Vector3(box->maxC.x, yCoord, box->maxC.z);
            toSplitPlane = Vector3(0.0f, yCoord - box->minC.y, 0.0f);
        }
        else    //longest dimension is z
        {
            //find length of potential box in z direction
            float zCoord = (zLength / MAX_BINS) * i + box->minC.z;
            box->plane_pos = zCoord;
            splitPlane->point = Vector3(box->maxC.x, box->maxC.y, zCoord);
            toSplitPlane = Vector3(0.0f, 0.0f, zCoord - box->minC.z);
        }
        
        objsInLeft = 0;
        objsInRight = 0;
        
        //determine if the objects in this box intersect with the left child and/or the right
        //child.
        for (int i = 0; i < box->numObjects; i++)
        {
            intersectLeft =
            box->objects[i]->boundingBox->testCollision(box->minC, splitPlane->point);
            intersectRight =
            box->objects[i]->boundingBox->testCollision(box->minC + toSplitPlane,
                                                        box->maxC);
            
            if (intersectLeft && intersectRight)
            {
                objsInLeft++;
                objsInRight++;
            }
            else if (intersectLeft)
            {
                objsInLeft++;
            }
            else if (intersectRight)
            {
                objsInRight++;
            }
        }
        
        saLeft = calcSurfaceArea(box->minC, splitPlane->point);
        saRight = calcSurfaceArea(box->minC + toSplitPlane, box->maxC);
        saParent = calcSurfaceArea(box->minC, box->maxC);
    
        c = (saLeft / saParent) * (float)objsInLeft + (saRight / saParent) * (float)objsInRight;
        
        //update children and cost if lower than current lowest.
        if( c < lowestCost || lowestCost == -1)
        {
            lowestCost = c;
            
            //delete old left child and right child
            delete box->left;
            delete box->right;
            
            //create new best potential candidate bounding boxes
            box->left = new bbox(splitPlane->point, box->minC, objsInLeft);
            box->right = new bbox(box->maxC, box->minC + toSplitPlane, objsInRight);
        }
    }
    
    //at this point, have found best split.
    
    int lCounter = 0;
    int rCounter = 0;
    //loop through every object in box we are splitting, see which child it falls in to
    for (int i = 0; i < box->numObjects; i++)
    {
        if (box->left->testCollision(box->objects[i]->boundingBox->minC,
                                     box->objects[i]->boundingBox->maxC))
        {
            box->left->objects[lCounter++] = box->objects[i];
        }
        
        if (box->right->testCollision(box->objects[i]->boundingBox->minC,
                                      box->objects[i]->boundingBox->maxC))
        {
            box->right->objects[rCounter++] = box->objects[i];
        }
    }
    
    if (lowestCost >= box->numObjects)
    {
        //printf("Cost of splitting is greater than cost of intersecting here. "
        //       "Num triangles in leaf: %u\n", box->numObjects);
        numLeaves++;
        //this is a leaf. Store index of first object, amount in
        box->isLeaf = true;
        
        //printf("At leaf, depth = %u\n", depth);
        
        //delete left child and right child. Should be a leaf.
        delete box->left;
        delete box->right;
        box->left = nullptr;
        box->right = nullptr;
        
        return;
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
}

bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    bbox* currentNode = root;

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    //intersection test with root
    if (!currentNode->intersect(ray, tMin, tMax))
        return false;
    else
        return intersectNode(root, minHit, ray, tMin, tMax);
    
}

bool BVH::intersectNode(bbox* box, HitInfo& minHit, const Ray& ray, float t_min, float t_max) const
{
    bbox *near, *far;
    Plane* splitPlane;
    HitInfo planeInfo;
    int depth = 0;
    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = t_max;
    
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
        
        if (splitPlane->intersect(planeInfo, ray))
            printf("Distance to plane: %f\n", planeInfo.t);
        else
            printf("Didn't intersect plane\n");
        
        if (planeInfo.t > t_max)
        {
            rayBoxIntersections++;
            intersectNode(near, minHit, ray, t_min, t_max);
        }
        else if (planeInfo.t < t_min)
        {
            rayBoxIntersections++;
            intersectNode(far, minHit, ray, t_min, t_max);
        }
        else
        {
            rayBoxIntersections += 2;
            intersectNode(near, minHit, ray, t_min, planeInfo.t);
            intersectNode(far, minHit, ray, planeInfo.t, t_max);
        }
    }
    else
    {
        //printf("intersected a leaf! Level: %u\n", depth);
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
                    minHit = tempMinHit;
                
                //printf("Hit an object!\n");
            }
        }
        
        if (hit && minHit.t < t_max)
        {
            //we hit something and know its the closest primitive in the box,
            //so return true
            return true;
        }
        else
            return false;
    }
    
    return false;
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
