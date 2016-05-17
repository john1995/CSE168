#include "bbox.h"
#include <algorithm> 

bbox::bbox()
{
    objects = nullptr;
}

bbox::bbox(Vector3 max, Vector3 min)
{
    maxC = max;
    minC = min;
    objects = nullptr;
}

bbox::bbox(Vector3 maxCorner, Vector3 minCorner, unsigned int numObjs) :
maxC(maxCorner), minC(minCorner), numObjects(numObjs)
{
    objects = new Object*[numObjects];
}

bbox::bbox(bbox& copy) : maxC(copy.maxC), minC(copy.minC), numObjects(copy.numObjects)
{
    objects = new Object*[numObjects];
}

bbox::~bbox()
{
    delete[] objects;
}

bool bbox::intersect(Ray r)
{
    Vector3 t1;
    Vector3 t2;
    Vector3 tmin;
    Vector3 tmax;
    
    t1.x = (minC.x - r.o.x) / r.d.x;
    t1.y = (minC.y - r.o.y) / r.d.y;
    t1.z = (minC.z - r.o.z) / r.d.z;

    t2.x = (maxC.x - r.o.x) / r.d.x;
    t2.y = (maxC.y - r.o.y) / r.d.y;
    t2.z = (maxC.z - r.o.z) / r.d.z;
    
    
    tmax.x = std::max(t1.x, t2.x);
    tmax.y = std::max(t1.y, t2.y);
    tmax.z = std::max(t1.z, t2.z);
    
    tmin.x = std::min(t1.x, t2.x);
    tmin.y = std::min(t1.y, t2.y);
    tmin.z = std::min(t1.z, t2.z);
    
    float t_min = std::max(tmin.x, std::max(tmin.y, tmin.z));
    float t_max = std::min(tmax.x, std::max(tmax.y, tmax.z));
    
    if (t_min < t_max)
    {
        //Distance to closest plane we hit.
        distFromRay = std::min(tmin.x, std::min(tmin.y, tmin.z));
        return true;
    }
    else
    {
        return false;
    }
}

float bbox::calcSurfaceArea()
{
    float height = maxC.y - minC.y;
    float width = maxC.x - minC.x;
    float length = maxC.z - minC.z;
    
    return 2 * (height * width) + 2 * (height * length) + 2 * (width * length);
}

float bbox::calcVolume()
{
    float height = maxC.y - minC.y;
    float width = maxC.x - minC.x;
    float length = maxC.z - minC.z;
    
    return length * width * height;
}

bool bbox::testCollision(bbox* other)
{
    if (maxC.x < other->minC.x ||
        maxC.y < other->minC.y ||
        maxC.z < other->minC.z ||
        minC.x > other->maxC.x ||
        minC.y > other->maxC.y ||
        minC.z > other->maxC.z)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void bbox::printBox(int level, bool right)
{
    //print the dimensions of this box
    
    if (!right)
    {
        printf("level %u, left dimensions - minC.x: %f, maxC.x: %f, minC.y: %f\n"
               "maxC.y: %f, minC.z: %f, maxC.z: %f\n", level, minC.x, maxC.x, minC.y,
               maxC.y, minC.z, maxC.z);
    }
    else
    {
        printf("level %u, left dimensions - minC.x: %f, maxC.x: %f, minC.y: %f\n"
               "maxC.y: %f, minC.z: %f, maxC.z: %f\n", level, minC.x, maxC.x, minC.y,
               maxC.y, minC.z, maxC.z);
    }
    
    if (children[0] != nullptr)
        children[0]->printBox(level + 1, false);
    
    if (children[1] != nullptr)
        children[1]->printBox(level + 1, true);
}

void bbox::drawNode()
{
    glBegin(GL_LINE_LOOP);
    glVertex3f(maxC.x, maxC.y, maxC.z);
    glVertex3f(maxC.x, maxC.y, minC.z);
    glVertex3f(minC.x, maxC.y, minC.z);
    glVertex3f(minC.x, maxC.y, maxC.z);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(maxC.x, minC.y, maxC.z);
    glVertex3f(maxC.x, minC.y, minC.z);
    glVertex3f(minC.x, minC.y, minC.z);
    glVertex3f(minC.x, minC.y, maxC.z);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex3f(maxC.x, maxC.y, maxC.z);
    glVertex3f(maxC.x, minC.y, maxC.z);
    glVertex3f(maxC.x, maxC.y, minC.z);
    glVertex3f(maxC.x, minC.y, minC.z);
    glVertex3f(minC.x, maxC.y, minC.z);
    glVertex3f(minC.x, minC.y, minC.z);
    glVertex3f(minC.x, maxC.y, maxC.z);
    glVertex3f(minC.x, minC.y, maxC.z);
    glEnd();
    
    if (children[0] != nullptr)
        children[0]->drawNode();
    
    if (children[1] != nullptr)
        children[1]->drawNode();
    
}
