#include "bbox.h"
#include <algorithm> 

bbox::bbox()
{
    objects = nullptr;
    numObjects = 0;
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

bool bbox::intersect(Ray r, float& tMin, float& tMax)
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
    
    if (t_min < t_max && tMin < t_min && tMax > t_max)
    {
        tMin = t_min;
        tMax = t_max;
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

bool bbox::testCollision(Vector3 minCorner, Vector3 maxCorner)
{
    if (maxC.x < minCorner.x ||
        maxC.y < minCorner.y ||
        maxC.z < minCorner.z ||
        minC.x > maxCorner.x ||
        minC.y > maxCorner.y ||
        minC.z > maxCorner.z)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void bbox::printBox(int level, bool rt)
{
    //print the dimensions of this box
    
    if (!rt)
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
    
    if (left != nullptr)
        left->printBox(level + 1, false);
    
    if (right != nullptr)
        right->printBox(level + 1, true);
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
    
    if (left != nullptr)
        left->drawNode();
    
    if (right != nullptr)
        right->drawNode();
    
}
