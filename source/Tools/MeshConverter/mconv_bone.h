

#ifndef __MCONV_BONE_H__
#define __MCONV_BONE_H__


#include "mconv_node.h"


namespace mconv
{
    class Bone : public Node
    {
    public:
        Bone(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Bone()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BONE;
        }

        FbxVector3  mTranslation;
        FbxVector4  mRotation;
        FbxVector3  mScale;
    };
}


#endif
