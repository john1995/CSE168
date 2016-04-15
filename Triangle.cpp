#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{
    m_material = new const Material[3];
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
    //Get the triangle by its index within the mesh it is a part of
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    
    const Vector3& a = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3& b = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3& c = m_mesh->vertices()[ti3.z]; //vertex c of triangle
    
    //Compute how far away ray's origin is from the triangle's plane
    //(or even if it hits the plane at all. Won't hit if ray parallel).
    //First need to get normal of triangle
    const Vector3& normal = (cross(b - a, c - a)).normalize();
    
    //Solve n dot (p - x) = 0, for p, where n = normal and x = ray
    //First check if n dot d is zero. If it is, ray is parallel to triangle
    float denom = dot(normal, r.d);
    
    if (denom == 0.0f)
        return false;
    
    result.t = dot(normal, a - r.o) / denom;
    
    if (result.t < tMin || result.t > tMax)
        return false;
    
    //Assign hit location to HitInfo result
    result.P = r.o + result.t * r.d;
    
    //Find smallest two components of triangle's normal vector
    int u, v;   //min indices
    float maxCmpnt = fmax(normal.x, fmax(normal.y, normal.z));
    
    if (maxCmpnt == normal.x) { u = 1; v = 2; }
    else if (maxCmpnt == normal.y) { u = 0; v = 2; }
    else { u = 0; v = 1; }
    
    //Compute barycentric coordinates of result.P relative to this triangle.
    //If alpha and beta both between 0 and 1, and alpha + beta <= 1, HIT
    //Check notes 4/5/16 for setup of linear equation and how Cramer's rule is used
    //to solve for each coordinate.
    
    //denominator in Cramer's Rule is same for both alpha and beta
    float denominator = (b[u] - a[u]) * (c[v] - a[v]) - (b[v] - a[v]) * (c[u] - a[u]);
    
    float alpha = ((result.P[u] - a[u]) * (c[v] - a[v]) -
                    (result.P[v] - a[v]) * (c[u] - a[u])) / denominator;
    
    float beta = ((b[u] - a[u]) * (result.P[v] - a[v]) -
                  (b[v] - a[v]) * (result.P[u] - a[u])) / denominator;
    
    if (alpha >= 0.0f && alpha <= 1.0f && beta >= 0.0f && beta <= 1.0f && alpha + beta <= 1.0f)
    {
        //There was a hit. Calculate Barycentric interpolation of normals so we can find
        //normal to shade with at hit point.
        //First, get vertex normals of vertices making up triangle.
        TriangleMesh::TupleI3 ti3 = m_mesh->nIndices()[m_index];
        
        const Vector3& normalA = m_mesh->normals()[ti3.x]; //Normal of A vert. of tri.
        const Vector3& normalB = m_mesh->normals()[ti3.y]; //Normal of B vert. of tri.
        const Vector3& normalC = m_mesh->normals()[ti3.z]; //Normal of C vert. of tri.
        
        //Interpolate and assign result.N
        //ARE ALPHA, BETA, AND GAMMA BEING MULTIPLIED BY THE RIGHT NORMALS?
        float gamma = 1.0f - alpha - beta;
        
        result.N = (gamma * normalA + alpha * normalB + beta * normalC).normalize();
        
        //Set material
        result.material = this->m_material; 
        
        return true;
    }
    else
        return false;
}
