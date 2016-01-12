

#ifndef __T3D_SG_RENDERABLE_H__
#define __T3D_SG_RENDERABLE_H__


#include "SG/Node/T3DSGNode.h"
#include "Resource/T3DMaterial.h"
#include "DataStruct/T3DIndexData.h"
#include "DataStruct/T3DVertexData.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGRenderable : public SGNode
    {
    protected:
        SGRenderable(uint32_t unID = E_NID_AUTOMATIC);

    public:
        virtual ~SGRenderable();

        void setMaterial(const MaterialPtr &material);
        MaterialPtr getMaterial()   { return mMaterial; }

        virtual const Matrix4 &getWorldMatrix() const;

        const VertexDataPtr &getVertices() { return mVertices; }
        const IndexDataPtr &getIndices()   { return mIndices; }

    protected:
        virtual void cloneProperties(SGNode *node) const override;

        virtual void updateTransform() override;

    protected:
        MaterialPtr     mMaterial;
        VertexDataPtr   mVertices;
        IndexDataPtr    mIndices;
    };
}


#endif  /*__T3D_SG_RENDERABLE_H__*/

