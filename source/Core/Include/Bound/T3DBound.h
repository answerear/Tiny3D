

#ifndef __T3D_BOUND_H__
#define __T3D_BOUND_H__


#include "T3DPrerequisites.h"
#include "Bound/T3DSphere.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Bound
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

        Bound(uint32_t unID, SGVisual *node);
        virtual ~Bound();

        virtual Type getType() const = 0;

        uint32_t getID() const;

        void setGroupID(uint32_t groupID);
        uint32_t getGroupID() const;

        const Sphere &getSphere() const;

        virtual bool test(const Bound &bound) const;

        const SGVisual *getNode() const;
        SGVisual *getNode();

        void setCollisionSource(bool isSource);
        bool isCollisionSource() const;

        void setEnable(bool enable);
        bool isEnable() const;

        virtual SGRenderable *getRenderable() = 0;

        virtual void setTransform(const Matrix4 &m);

        virtual Bound *clone() = 0;

    protected:
        virtual bool testSphere(const SphereBound &bound) const = 0;
        virtual bool testAabb(const AabbBound &bound) const = 0;
        virtual bool testObb(const ObbBound &bound) const = 0;
        virtual bool testFrustum(const FrustumBound &bound) const = 0;

        virtual void updateBound(const Matrix4 &m) = 0;

    protected:
        Sphere  mSphere;

    private:
        SGVisual    *mNode;
        uint32_t    mID;
        uint32_t    mGroupID;
        bool        mMovable;
        bool        mCollisionSource;
        bool        mEnable;
    };
}


#include "T3DBound.inl"


#endif  /*__T3D_BOUND_H__*/
