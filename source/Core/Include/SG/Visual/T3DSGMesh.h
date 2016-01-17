
#ifndef __T3D_SG_MESH_H__
#define __T3D_SG_MESH_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGMesh : public SGVisual
    {
    public:
        enum MeshType
        {
            E_TYPE_NONE = 0,        /// 空mesh
            E_TYPE_BOX,             /// 内置立方体mesh
            E_TYPE_SPHERE,          /// 内置球体mesh
            E_TYPE_PLANE,           /// 内置平面mesh
            E_TYPE_CUSTOM_MESH,     /// 外部加载的mesh
        };

        static SGMeshPtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGMesh();

        bool loadBox();

        virtual Type getNodeType() const override;

    protected:
        SGMesh(uint32_t unID = E_NID_AUTOMATIC);

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        virtual SGNodePtr clone() const override;
        virtual void cloneProperties(SGNode *node) const override;
    };
}


#endif  /*__T3D_SG_MESH_H__*/
