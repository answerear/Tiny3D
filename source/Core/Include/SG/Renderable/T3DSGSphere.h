

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

        bool loadSphere();

        virtual Type getNodeType() const override;
        virtual SGNodePtr clone() const override;

        void setRadius(Real radius);
        Real getRadius() const  { return mRadius; }

    protected:
        SGSphere(uint32_t uID = E_NID_AUTOMATIC);

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        virtual void cloneProperties(const SGNodePtr &node) const override;

        void loadVertices(Vector3 *vertices, size_t vertexCount);
        void loadIndices(uint16_t *indices, size_t indexCount);

        Vector3 getPoint(Real xyStep, Real zStep) const;

        enum
        {
            MAX_STACKS = 18,
            MAX_SLICES = 18,
            MAX_VERTICES = (MAX_STACKS + 1) * (MAX_SLICES + 1),
            MAX_TRIANGLES = MAX_STACKS * MAX_SLICES * 2,
        };

        Vector3 mVertices[MAX_VERTICES];
        Real    mRadius;
    };
}



#endif  /*__T3D_SG_SPHERE_H__*/
