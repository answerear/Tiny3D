

#ifndef __T3D_BONE_H__
#define __T3D_BONE_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DNode.h"
#include "T3DMatrix4.h"
#include "T3DTransform.h"


namespace Tiny3D
{
    class Bone : public Node
    {
    public:
        virtual ~Bone();

        static BonePtr create(const String &name, const Matrix4 &offsetMatrix, const Matrix4 &localMatrix);

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        void setTranslation(const Vector3 &pos)
        {
            mTranslation = pos;
            setDirty(true, true);
        }

        void setOrientation(const Quaternion &orientation)
        {
            mOrientation = orientation;
            setDirty(true, true);
        }

        void setScaling(const Vector3 &scaling)
        {
            mScaling = scaling;
            setDirty(true, true);
        }

        void updateBone();

        void setDirty(bool isDirty, bool recursive = false);
        bool isDirty() const { return mIsDirty; }

        const Transform &getCombineTransform();
        const Matrix4 &getBindPoseMatrix();

        const Matrix4 &getInverseBoneMatrix();

    protected:
        Bone();
        Bone(const String &name, const Matrix4 &bindposeMatrix, const Matrix4 &localMatrix);

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual void onAttachParent(const NodePtr &parent) override;

    protected:
        Vector3     mTranslation;
        Vector3     mScaling;
        Quaternion  mOrientation;

        Transform   mCombineTransform;

        Matrix4     mInverseBoneMatrix;
        Matrix4     mBindposeMatrix;

        bool        mInverseDirty;
        bool        mIsDirty;
    };
}

#endif  /*__T3D_BONE_H__*/
