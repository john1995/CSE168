#include "TriangleMesh.h"
#include "Triangle.h"
#include "Scene.h"


TriangleMesh::TriangleMesh() :
    m_normals(0),
    m_vertices(0),
    m_texCoords(0),
    m_normalIndices(0),
    m_vertexIndices(0),
    m_texCoordIndices(0)
{

}

void TriangleMesh::translate(const Vector3 t)
{
    //loop throug all verts
    for (int i = 0; i < m_numVerts; ++i)
    {
        //translate
        vertices()[i] += t;
    }
}

void TriangleMesh::rotateX(float radX)
{
    //make an x rotation matrix
    
}

void TriangleMesh::rotateY(float radY)
{
    
}

void TriangleMesh::rotateZ(float radZ)
{
    
}

TriangleMesh::~TriangleMesh()
{
    delete [] m_normals;
    delete [] m_vertices;
    delete [] m_texCoords;
    delete [] m_normalIndices;
    delete [] m_vertexIndices;
    delete [] m_texCoordIndices;
}