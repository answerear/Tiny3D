

#ifndef __T3D_SG_BONE_H__
#define __T3D_SG_BONE_H__


#include "SG/Node/T3DSGTransformNode.h"


namespace Tiny3D
{
    class SGBone : public SGTransformNode
    {
    public:
        virtual ~SGBone();

        static SGBonePtr create(ObjectPtr data = nullptr, uint32_t unID = E_NID_AUTOMATIC);

        uint16_t getParentBone() const;
        ObjectPtr getBoneData() const { return mBoneData; }

        void updateBone();

        virtual void updateTransform() override;
        virtual Type getNodeType() const override;

        virtual NodePtr clone() const override;

    protected:
        SGBone(uint32_t unID = E_NID_AUTOMATIC);

        virtual bool init(ObjectPtr data);

    protected:
        ObjectPtr   mBoneData;
    };
}


#endif  /*__T3D_SG_BONE_H__*/
