#ifndef CSE168_TRIANGLE_H_INCLUDED
#define CSE168_TRIANGLE_H_INCLUDED

#include "Object.h"


/*
    The Triangle class stores a pointer to a mesh and an index into its
    triangle array. The mesh stores all data needed by this Triangle.
*/
class Triangle : public Object
{
public:
    Triangle(TriangleMesh * m = 0, unsigned int i = 0);
    virtual ~Triangle();

    void setIndex(unsigned int i) {m_index = i;}
    void setMesh(TriangleMesh* m) {m_mesh = m;}
    TriangleMesh* getMesh()   { return m_mesh; }
    unsigned int getIndex()     { return m_index; }
    
    virtual void renderGL();
    virtual bool intersect(HitInfo& result, const Ray& ray,
                           float tMin = 0.0f, float tMax = MIRO_TMAX);
    
    //Calculate centroid of triangle
    Vector3 calcCentroid();
    
protected:
    TriangleMesh* m_mesh;   //Ptr to the mesh this triangle is a part of
    unsigned int m_index;   //index of the triangle in the TriangleMesh

};

#endif // CSE168_TRIANGLE_H_INCLUDED
