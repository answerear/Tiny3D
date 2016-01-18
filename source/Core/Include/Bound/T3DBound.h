

#ifndef __T3D_BOUND_H__
#define __T3D_BOUND_H__


#include "Misc/T3DObject.h"
#include "Math/T3DSphere.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Bound : public Object
    {
    public:
        enum Type
        {
            E_BT_NONE = 0,
            E_BT_SPHERE,
            E_BT_AABB,
            E_BT_OBB,
            E_BT_FRUSTUM,
        };

        virtual ~Bound();

        virtual Type getType() const = 0;

        uint32_t getID() const;

        void setGroupID(uint32_t groupID);
        uint32_t getGroupID() const;

        const Sphere &getSphere() const;

        virtual bool test(const BoundPtr &bound) const;

        const SGVisualPtr getVisualNode() const;
        SGVisualPtr getVisualNode();

        void setCollisionSource(bool isSource);
        bool isCollisionSource() const;

        void setEnable(bool enable);
        bool isEnable() const;

        virtual SGRenderablePtr getRenderable() = 0;

        virtual void setTransform(const Transform &transform);

        virtual BoundPtr clone() const = 0;

    protected:
        Bound(uint32_t unID, SGVisual *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const = 0;
        virtual bool testAabb(const AabbBoundPtr &bound) const = 0;
        virtual bool testObb(const ObbBoundPtr &bound) const = 0;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const = 0;

        virtual void updateBound(const Transform &transform) = 0;

        virtual void cloneProperties(const BoundPtr &bound) const;

    protected:
        Sphere  mSphere;

    private:
        SGVisual        *mNode;

        uint32_t    mID;
        uint32_t    mGroupID;
        bool        mMovable;
        bool        mCollisionSource;
        bool        mEnable;
    };
}


#include "T3DBound.inl"


#endif  /*__T3D_BOUND_H__*/
