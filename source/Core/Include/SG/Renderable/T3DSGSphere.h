

#ifndef __T3D_SG_SPHERE_H__
#define __T3D_SG_SPHERE_H__


#include "SG/Renderable/T3DSGRenderable.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGSphere : public SGRenderable
    {
    public:
        static SGSpherePtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGSphere();

        virtual Type getNodeType() const override;

        virtual SGNodePtr clone() const override;

    protected:
        SGSphere(uint32_t uID = E_NID_AUTOMATIC);

        virtual void cloneProperties(const SGNodePtr &node) const override;

    };
}



#endif  /*__T3D_SG_SPHERE_H__*/
