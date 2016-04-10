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
    //First get vertices of triangle
    
    //Get the triangle by its index within the mesh it is a part of
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    
    const Vector3& a = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3& b = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3& c = m_mesh->vertices()[ti3.z]; //vertex c of triangle
    
    //Compute how far away ray's origin is from the triangle's plane
    //(or even if it hits the plane at all. Won't hit if ray parallel).
    //First need to get normal of triangle
    const Vector3& normal = Vector3::cross(b - a, c - a);
    
    //fill in shading normal in HitInfo result
    result.N = normal;
    
    //Solve n dot (p - x) = 0, for p, where n = normal and x = ray
    //First check if n dot d is zero. If it is, ray is parallel to triangle
    float denom = dot(normal, r.d);
    
    if (denom == 0.0f)
        return false;
    
    result.t = Vector3::dot(normal, a - r.o) / denom;
    
    //Assign hit location to HitInfo result
    result.P = r.o + result.t * r.d;
    
    //Compute barycentric coordinates of result.P relative to this triangle.
    //If alpha and beta both between 0 and 1, and alpha + beta <= 1, HIT
    //Check notes 4/5/16 for setup of linear equation and how Cramer's rule is used
    //to solve for each coordinate.
    float alpha = 
    
}
