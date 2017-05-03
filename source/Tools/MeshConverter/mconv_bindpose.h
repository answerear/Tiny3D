

#ifndef __MCONV_BINDPOSE_H__
#define __MCONV_BINDPOSE_H__


#include "mconv_node.h"


namespace mconv
{
    class BindPose : public Node
    {
    public:
        BindPose(const String &ID)
            : Node(ID)
        {

        }

        virtual ~BindPose()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BINDPOSE;
        }
    };
}


#endif  /*__MCONV_BINDPOSE_H__*/
