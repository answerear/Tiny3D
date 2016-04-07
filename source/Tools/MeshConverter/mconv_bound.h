

#ifndef __MCONV_BOUND_H__
#define __MCONV_BOUND_H__


#include "mconv_node.h"


namespace mconv
{
    class SphereBound : public Node
    {
    public:
        SphereBound(const String &ID)
            : Node(ID)
            , mCenterX(0.0f)
            , mCenterY(0.0f)
            , mCenterZ(0.0f)
            , mRadius(1.0f)
        {

        }

        virtual ~SphereBound()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BOUND_SPHERE;
        }

        float   mCenterX, mCenterY, mCenterZ;
        float   mRadius;
    };

    class AabbBound : public Node
    {
    public:
        AabbBound(const String &ID)
            : Node(ID)
            , mMinX(-1.0f)
            , mMaxX(1.0f)
            , mMinY(-1.0f)
            , mMaxY(1.0f)
            , mMinZ(-1.0f)
            , mMaxZ(1.0f)
        {

        }

        virtual ~AabbBound()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BOUND_AABB;
        }

        float   mMinX;
        float   mMaxX;
        float   mMinY;
        float   mMaxY;
        float   mMinZ;
        float   mMaxZ;
    };
}

#endif
