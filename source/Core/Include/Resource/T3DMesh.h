

#ifndef __T3D_MESH_H__
#define __T3D_MESH_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SubMesh : public Object
    {
    public:
        SubMesh();
        virtual ~SubMesh();

        
    };

    class T3D_ENGINE_API Mesh : public Resource
    {
    public:
        static MeshPtr create(const String &name);

        virtual ~Mesh();

        virtual Type getType() const override;

    protected:
        Mesh(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;
    };
}

#endif  /*__T3D_MESH_H__*/
