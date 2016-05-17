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
    box->children[0] = nullptr;
    box->children[1] = nullptr;
    
    printf("In build_recursive\n");
    printf("Depth: %u\n", depth);
    printf("Volume of box to potentially split: %f\n", box->calcVolume());
    //Stop when number of objects in box <= 8
    if (box->numObjects <= 8 || depth > 200 || box->calcVolume() < 1.0f)
    {
        printf("Stopping recursion here: num objects in leaf: %u\n", box->numObjects);
        numLeaves++;
        //this is a leaf. Store index of first object, amount in
        box->isLeaf = true;
        //printf("8 or fewer objects in box, returning\n");
        
        return;
    }
    
    //temporary boxes for calculating cost.
    bbox *tempLeft = new bbox();
    bbox *tempRight = new bbox();
    tempLeft->numObjects = 0;
    tempRight->numObjects = 0;
    
    int overlaps[MAX_BINS]; //stores how many tris overlap bins
    overlaps[0] = 0;
    //overlaps[1] = num tris that sit in both bin 0 and bin 1.
    
    //Initialize dimensions of each temp box to be dimensions of parent. Only values that
    //will ever change are the lengths of the boxes along the largest axis.
    tempLeft->minC = box->minC;
    tempLeft->maxC = box->maxC;
    tempRight->minC = box->minC;
    tempRight->maxC = box->maxC;
    
    int k;  //variable that stores coordinate we will be using to find bin for tri (x, y, or z)
    
    //calculate the largest dimension of this bounding box. Will determine which
    //dimensions we will be dividing bins on. Got to be better way to do this.
    float xLength = box->maxC.x - box->minC.x;
    float yLength = box->maxC.y - box->minC.y;
    float zLength = box->maxC.z - box->minC.z;
    float longestDimLen;
    float minD = 0.0f;
    
    if (xLength > yLength && xLength > zLength)
    {
        k = 0;
        longestDimLen = xLength;
        minD = box->minC.x;
        //printf("Longest dimension is x, length %f\n", xLength);
    }
    else if (yLength > xLength && yLength > zLength)
    {
        k = 1;
        longestDimLen = yLength;
        minD = box->minC.y;
        //printf("Longest dimension is y, length %f\n", yLength);
    }
    else
    {
        k = 2;
        longestDimLen = zLength;
        minD = box->minC.z;
        //printf("Longest dimension is z, length %f\n", zLength);
    }
    
    /**********************************************************************/
    
    //holds lowest cost
    float lowestCost = -1;   //starts at some impossibly high cost.
    int bestSplit = -1;                 //best index to divide primitives at.
    int bestAmtInLft = 0;               //amount in left box for optimal split.
    
    //printf("numBins: %u\n", numBins);
    
    Object* temp;
    
    //precompute values to make bin sorting faster
    float k1 = MAX_BINS / longestDimLen;
    
    bool intersectLeft, intersectRight;
    int total = 0;
    
    //int count = 0;
    //for (int a = 0; a < numBins; a++)
    //{
    //    count += binCount[a];
    //}
    
    bool minRight = false;
    
    //printf("Amount of triangles in scene: %u\n", count);
    
    //holds how many objects are in left box.
    //int objsInLeft = 0;
    //int objsInRight = box->numObjects;
    
    float c = 0.0f;    //where lowest cost will be stored
    
    //Go through whole list of bins and find optimal place to split.
    //IN THIS LOOP, FIND STARTING LEFT BOX WHERE THERE ARE OBJECTS, AND ENDING RIGHT BOX
    //WHERE THERE AREN'T OBJECTS AFTER (LATER)
    for(int i = 1; i < MAX_BINS; ++i)
    {
        //objsInLeft += binCount[i - 1];
        //objsInRight = total - binCount[i - 1];   //add back in overlaps from the other side
        
        //First need to adjust min corners and max corners of left and right boxes.
        if (k == 0) //longest dimension is x
        {
            //find length of potential box in x direction
            float xCoord = (xLength / MAX_BINS) * i + tempLeft->minC.x;
            tempLeft->maxC.x = xCoord;
            tempRight->minC.x = xCoord;
        }
        else if (k == 1)   //longest dimension is y
        {
            //find length of potential box in y direction
            float yCoord = (yLength / MAX_BINS) * i + tempLeft->minC.y;
            tempLeft->maxC.y = yCoord;
            tempRight->minC.y = yCoord;
        }
        else    //longest dimension is z
        {
            //find length of potential box in x direction
            float zCoord = (zLength / MAX_BINS) * i + tempLeft->minC.z;
            tempLeft->maxC.z = zCoord;
            tempRight->minC.z = zCoord;
        }
        
        tempLeft->numObjects = 0;
        tempRight->numObjects = 0;
        
        //loop through every object in node, find which bin each object is in.
        for (int i = 0; i < box->numObjects; i++)
        {
            temp = box->objects[i];
            //printf("Address of current object: %p\n", temp);
            //printf("getting tri at index %u\n", i);
            //printf("Number of objects in this box: %u\n", box->numObjects);
            //printf("triangle %u centroid along %u is %f\n", i, k, temp->centroid[k]);
            //printf("length of longest dimension: %f\n", longestDimLen);
            intersectLeft = temp->boundingBox->testCollision(tempLeft);
            intersectRight = temp->boundingBox->testCollision(tempRight);
            
            if (intersectLeft && intersectRight)
            {
                tempLeft->numObjects++;
                tempRight->numObjects++;
                //overlaps[rBoxID]++;
                total += 2;
            }
            else if (intersectLeft)
            {
                tempLeft->numObjects++;
                total++;
            }
            else if (intersectRight)
            {
                tempRight->numObjects++;
                total++;
            }
        }
        
        //printf("numObjects in left child: %u, numObjects in right child: %u\n",
        //       tempLeft->numObjects,
        //       tempRight->numObjects);
        
        //tempLeft.numObjects = objsInLeft;
        //tempRight.numObjects = objsInRight;
        
        //Compute cost of split
        /*printf("Surface area of left box: %f\n", tempLeft.calcSurfaceArea());
        printf("Surface area of right box: %f\n", tempRight.calcSurfaceArea());*/
        //printf("Num of objects in left box: %u\n", tempLeft.numObjects);
        //printf("Num of objects in right box: %u\n", tempRight.numObjects);
        c = (tempLeft->calcSurfaceArea() / box->calcSurfaceArea()) * (float)tempLeft->numObjects +
        (tempRight->calcSurfaceArea() / box->calcSurfaceArea()) * (float)tempRight->numObjects;
        
        //printf("Cost: %f\n", c);
        
        if( c < lowestCost || lowestCost == -1)
        {
            lowestCost = c;
            bestSplit = i;
            //printf("new lowest cost! best split: %u\n", bestSplit);
            //bestAmtInLft = objsInLeft;
            //printf("Best amount of triangles in left box: %u\n", bestAmtInLft);
            
            //delete old left child and right child
            delete box->children[0];
            delete box->children[1];
            
            //create new best potential candidate bounding boxes
            box->children[0] = new bbox(*tempLeft);
            box->children[1] = new bbox(*tempRight);
        }
    }
    
    int lCounter = 0;
    int rCounter = 0;
    //loop through every object in box we are splitting, see which child it falls in to
    for (int i = 0; i < box->numObjects; i++)
    {
        //printf("checking if primitive %u in box intersects with child boxes\n", i);
        //Can potentially be added to both boxes.
        if (box->children[0]->testCollision(box->objects[i]->boundingBox))
        {
            box->children[0]->objects[lCounter++] = box->objects[i];
        }
        
        if (box->children[1]->testCollision(box->objects[i]->boundingBox))
        {
            box->children[1]->objects[rCounter++] = box->objects[i];
        }
    }
    
    //printf("Amount of nodes in left box at best place to split: %u\n", bestAmtInLft);
    
    if (lowestCost >= box->numObjects)
    {
        printf("Cost of splitting is greater than cost of intersecting here. "
               "Num triangles in leaf: %u\n", box->numObjects);
        numLeaves++;
        //this is a leaf. Store index of first object, amount in
        box->isLeaf = true;
        
        //delete left child and right child. Should be a leaf.
        delete box->children[0];
        delete box->children[1];
        box->children[0] = nullptr;
        box->children[1] = nullptr;
        
        return;
    }
    
    //Recursively split.
    numNodes++;
    splitNode(box->children[0], depth + 1);
    
    if (!box->children[0]->isLeaf)
    {
        delete[] box->children[0]->objects;
    }
    
    numNodes++;
    splitNode(box->children[1], depth + 1);
    
    if (!box->children[1]->isLeaf)
    {
        delete[] box->children[1]->objects;
    }
    
    delete tempLeft;
    delete tempRight;
}


bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    bbox* currentNode = root;
    
    //vector being used as a stack to hold nodes that may need to be checked later.
    std::vector<bbox*> nodeStack;

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    //intersection test with root
    if (!currentNode->intersect(ray))
        return false;
    
    bool intersectChildOne, intersectChildTwo;
    
    //loop will terminate if we hit primitive or if we miss bounding box/primitive.
    while (1)
    {
        //printf("Going to check intersections\n");
        //printf("Done checking intersections\n");
        
        //if not a leaf, check for intersection with both child boxes
        if (!currentNode->isLeaf)
        {
            intersectChildOne = currentNode->children[0]->intersect(ray);
            intersectChildTwo = currentNode->children[1]->intersect(ray);
            
            //case: both child bounding boxes are intersected.
            if (intersectChildOne &&
                intersectChildTwo)
            {
                rayBoxIntersections += 2;
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
            else if (!intersectChildOne &&
               intersectChildTwo)
            {
                rayBoxIntersections++;
                currentNode = currentNode->children[1];
                continue;
            }
            //case: only left child is intersected
            else if (intersectChildOne &&
                     !intersectChildTwo)
            {
                rayBoxIntersections++;
                currentNode = currentNode->children[0];
                continue;
            }
            //case: neither children node intersected
            else { /* Do nothing */ }
        }
        else
        {
            //We have reached a leaf. Check primitives inside.
            for (int i = 0; i < currentNode->numObjects; i++)
            {
                if (currentNode->objects[i]->intersect(tempMinHit, ray, tMin, tMax))
                {
                    rayTriIntersections++;
                    hit = true;
                    //Only update hitinfo if object we hit is closer to camera
                    if (tempMinHit.t < minHit.t)
                        minHit = tempMinHit;
                    
                    //printf("Hit an object!\n");
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
        
        //If we make it here, we haven't already returned from a hit, and all
        //child nodes that were farther away have been tested. Therefore, there
        //was no hit, and we can return false.
        if (nodeStack.empty())
        {
            //printf("No intersection!\n");
            return false;
        }
    }
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
