

#ifndef __T3D_SG_RENDERABLE_H__
#define __T3D_SG_RENDERABLE_H__


#include "SG/Node/T3DSGNode.h"


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

        Transform getWorldTransform();

        VertexListPtr getVertices() { return mVertices; }
        IndexListPtr getIndices()   { return mIndices; }

    protected:
        virtual void cloneProperties(SGNode *node) const override;

        virtual void updateTransform() override;

    protected:
        MaterialPtr     mMaterial;
        VertexListPtr   mVertices;
        IndexListPtr    mIndices;
    };
}


#endif  /*__T3D_SG_RENDERABLE_H__*/

