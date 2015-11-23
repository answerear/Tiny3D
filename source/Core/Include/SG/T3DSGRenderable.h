

#include "SG/T3DSGNode.h"


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

        virtual void update();

    protected:
        Material *mMaterial;
    };
}
