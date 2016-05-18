

#ifndef __T3D_SG_MESH_H__
#define __T3D_SG_MESH_H__


#include "SG/Renderable/T3DSGGeometry.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGMesh : public SGGeometry
    {
    public:
        static SGMeshPtr create(const ObjectPtr &geometryData, bool isVertexShared, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGMesh();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

    protected:
        SGMesh(uint32_t uID = E_NID_AUTOMATIC);

        virtual bool init(const ObjectPtr &geometryData, bool isVertexShared);

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

    protected:
        ObjectPtr   mGeometryData;
        MaterialPtr mMaterial;
        bool        mIsVertexShared;
    };
}


#endif  /*__T3D_SG_MESH_H__*/
