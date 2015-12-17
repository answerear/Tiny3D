
#ifndef __T3D_SG_TRANSFORM_NODE_H__
#define __T3D_SG_TRANSFORM_NODE_H__


#include "SG/T3DSGNode.h"
#include "Math/T3DMath.h"
#include "Math/T3DVector3.h"
#include "Math/T3DMatrix3.h"
#include "Math/T3DMatrix4.h"
#include "Math/T3DTransform.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGTransformNode : public SGNode
    {
    public:
        SGTransformNode(uint32_t unID = E_NID_AUTOMATIC);
        virtual ~SGTransformNode();

        virtual Type getNodeType() const override;

        void setPosition(const Vector3 &rkPos);
        void setPosition(Real x, Real y, Real z);
        const Vector3 &getPosition() const;

        void setOrientation(const Quaternion &rkQ);
        void setOrientation(Real w, Real x, Real y, Real z);
        const Quaternion &getOrientation() const;

        void setScale(const Vector3 &rkScale);
        void setScale(Real x, Real y, Real z);
        const Vector3 &getScale() const;

        void translate(const Vector3 &rkOffset);
        void translate(Real x, Real y, Real z);
        void translate(const Matrix3 &axis, const Vector3 &rkOffset);
        void translate(const Matrix3 &axis, Real x, Real y, Real z);

        void rotate(const Quaternion &rkQ);
        void rotate(const Vector3 &axis, const Radian &radians);
        void rotate(const Vector3 &axis, const Degree &degrees);

        void pitch(const Degree &degrees);
        void yaw(const Degree &degrees);
        void roll(const Degree &degrees);

        void pitch(const Radian &radians);
        void yaw(const Radian &radians);
        void roll(const Radian &radians);

        void scale(const Vector3 &rkScale);
        void scale(Real x, Real y, Real z);

        /**
         * @brief This method only be used by SGCamera for child.
         * @param [in] pos : 
         * @param [in] obj : 
         * @param [in] up : 
         */
        void lookAt(const Vector3 &pos, const Vector3 &obj, const Vector3 &up);

        virtual const Transform &getLocalToWorldTransform();
//         Transform getWorldToLocalTransform();

        virtual void addChild(SGNode *node) override;

    protected:
        virtual void onAttachParent(SGNode *parent) override;
        virtual void onDetachParent(SGNode *parent) override;

        virtual void updateTransform() override;

        virtual SGNode *clone() override;
        virtual void cloneProperties(SGNode *node) override;

    private:
        Vector3     mPosition;
        Quaternion  mOrientation;
        Vector3     mScale;

        Transform   mWorldTransform;
    };
}


#include "T3DSGTransformNode.inl"


#endif  /*__T3D_SG_TRANSFORM_NODE_H__*/
