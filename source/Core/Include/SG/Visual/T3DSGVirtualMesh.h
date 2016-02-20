

#ifndef __T3D_SG_VIRTUAL_MESH_H__
#define __T3D_SG_VIRTUAL_MESH_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGVirtualMesh : public SGVisual
    {
    public:
        static SGVirtualMeshPtr create(uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGVirtualMesh();

        virtual Type getNodeType() const override;
        virtual SGNodePtr clone() const override;

    protected:
        SGVirtualMesh(uint32_t uID = E_NID_AUTOMATIC);

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;
    };
}


#endif  /*__T3D_SG_VIRTUAL_MESH_H__*/
