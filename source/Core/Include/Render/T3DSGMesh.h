

#ifndef __T3D_SG_MESH_H__
#define __T3D_SG_MESH_H__


#include "Render/T3DSGGeometry.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGMesh : public SGGeometry
    {
    public:
        static SGMeshPtr create(VertexDataPtr vertexData, ObjectPtr submeshData, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGMesh();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        const String &getSubMeshName() const;

    protected:
        SGMesh(uint32_t uID = E_NID_AUTOMATIC);

        virtual bool init(VertexDataPtr vertexData, ObjectPtr submeshData);

        virtual void updateTransform() override;

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

    protected:
        ObjectPtr   mSubMeshData;
        MaterialPtr mMaterial;

        VertexDataPtr   mVertexData;
        IndexDataPtr    mIndexData;
    };
}


#endif  /*__T3D_SG_MESH_H__*/
