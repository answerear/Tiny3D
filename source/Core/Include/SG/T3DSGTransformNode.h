

#ifndef __T3D_SG_TRANSFORM_NODE_H__
#define __T3D_SG_TRANSFORM_NODE_H__


#include "SG/T3DSGNode.h"
#include "Math/T3DVector3.h"
#include "Math/T3DQuaternion.h"
#include "Math/T3DMath.h"
#include "Math/T3DMatrix3.h"
#include "Math/T3DMatrix4.h"
#include "Bound/T3DSphereBound.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGTransformNode : public SGNode
    {
    public:
        SGTransformNode(uint32_t unID = E_NID_AUTOMATIC);
        virtual ~SGTransformNode();

        void setPosition(const Vector3 &rkPos);
        void setPosition(Real x, Real y, Real z);
        const Vector3 &getPosition() const;

        void setOrientation(const Quaternion &rkQ);
        void setOrientation(Real w, Real x, Real y, Real z);
        const Quaternion &getOrientation() const;

        void setScale(const Vector3 &rkScale);
        void setScale(Real x, Real y, Real z);
        const Vector3 &getScale() const;

        void translate(const Vector3 &rkOffet);
        void translate(Real x, Real y, Real z);
        void translate(const Matrix3 &axis, const Vector3 &rkOffset);
        void translate(const Matrix3 &axis, Real x, Real y, Real z);

        void roll(const Degree &degrees);
        void pitch(const Degree &degrees);
        void yaw(const Degree &degrees);

        void rotate(const Vector3 &axis, const Degree &degrees);
        void rotate(const Quaternion &rkQ);

        void scale(const Vector3 &rkScale);
        void scale(Real x, Real y, Real z);

        virtual Bound *getBoundingBox();

        virtual const Matrix4 &getLocalToWorldTransform() const;
        Matrix4 getWorldToLocalTransform() const;

        void setVisible(bool visible);
        bool isVisible() const;

        bool isDirty() const;

    protected:
        virtual void update();

        virtual void onAttachParent(SGNode *parent);

    private:
        Vector3     mPosition;
        Quaternion  mOrientation;
        Vector3     mScale;

        mutable Matrix4     mWorldTransform;

        bool        mIsDirty;
        bool        mIsVisible;
    };
}


#include "T3DSGTransformNode.inl"


#endif  /*__T3D_SG_TRANSFORM_NODE_H__*/
