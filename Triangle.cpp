#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}


void
Triangle::renderGL()
{
    //The individual triangle only holds its index within the mesh.
    //The mesh holds its vertices and normals, as well as the indices of each.
    
    //Get the triangle by its index within the mesh it is a part of
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    
    //Get each of its vertices through their indices in the mesh, which
    //are stored in a custom-defined 'tuple' within the triangle mesh
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r,float tMin, float tMax)
{
    return false;
}
