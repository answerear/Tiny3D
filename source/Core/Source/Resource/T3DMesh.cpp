

#include "Resource/T3DMesh.h"


namespace Tiny3D
{
    SubMesh::SubMesh()
    {

    }

    SubMesh::~SubMesh()
    {

    }

    ////////////////////////////////////////////////////////////////////////////

    MeshPtr Mesh::create(const String &name)
    {
        MeshPtr mesh = new Mesh(name);
        mesh->release();
        return mesh;
    }

    Mesh::Mesh(const String &name)
        : Resource(name)
    {

    }

    Mesh::~Mesh()
    {

    }

    Resource::Type Mesh::getType() const
    {
        return E_TYPE_MESH;
    }

    bool Mesh::load()
    {
        return true;
    }

    void Mesh::unload()
    {
        Resource::unload();
    }

    ResourcePtr Mesh::clone() const
    {
        MeshPtr mesh = create(mName);
        return mesh;
    }
}
