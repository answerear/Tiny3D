

#ifndef __MCONV_SKELETON_H__
#define __MCONV_SKELETON_H__


#include "mconv_node.h"


namespace mconv
{
    class Skeleton : public Node
    {
    public:
        Skeleton(const String &ID)
            : Node(ID)
            , mBoneCount(0)
        {

        }

        virtual ~Skeleton()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SKELETON;
        }

        uint16_t    mBoneCount;
    };
}

#endif  /*__MCONV_SKELETON_H__*/
