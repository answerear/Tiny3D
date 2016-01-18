

#ifndef __T3D_SG_RENDERABLE_H__
#define __T3D_SG_RENDERABLE_H__


#include "SG/Node/T3DSGNode.h"
#include "Resource/T3DMaterial.h"
#include "DataStruct/T3DIndexData.h"
#include "DataStruct/T3DVertexData.h"
#include "Render/T3DRenderer.h"


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

        Renderer::PrimitiveType getPrimitiveType() const    { return mPrimitiveType; }
        const VertexDataPtr &getVertexData() { return mVertexData; }
        const IndexDataPtr &getIndexData()   { return mIndexData; }

    protected:
        virtual void cloneProperties(const SGNodePtr &node) const override;

        virtual void updateTransform() override;

    protected:
        MaterialPtr             mMaterial;
        Renderer::PrimitiveType mPrimitiveType;
        VertexDataPtr           mVertexData;
        IndexDataPtr            mIndexData;
    };
}


#endif  /*__T3D_SG_RENDERABLE_H__*/

