

#ifndef __T3D_SG_BONE_H__
#define __T3D_SG_BONE_H__


#include "SG//T3DSGTransformNode.h"


namespace Tiny3D
{
    class SGBone : public SGTransformNode
    {
    public:
        virtual ~SGBone();

        static SGBonePtr create(ObjectPtr data = nullptr, uint32_t unID = E_NID_AUTOMATIC);

        uint16_t getParentBone() const;
        ObjectPtr getBoneData() const { return mBoneData; }

        virtual void updateTransform() override;

        virtual Type getNodeType() const override;

        virtual NodePtr clone() const override;

        const Matrix4 &getFinalMatrix() const { return mFinalMatrix; }

    protected:
        SGBone(uint32_t unID = E_NID_AUTOMATIC);

        virtual bool init(ObjectPtr data);

    protected:
        ObjectPtr   mBoneData;
        Matrix4     mFinalMatrix;
    };
}


#endif  /*__T3D_SG_BONE_H__*/
