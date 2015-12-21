

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

        void setMaterial(Material *material);
        Material *getMaterial();

    protected:
        virtual void cloneProperties(SGNode *node) const override;

        virtual void updateTransform() override;

    protected:
        Material *mMaterial;
    };
}


#endif  /*__T3D_SG_RENDERABLE_H__*/

