

#ifndef __MCONV_SKIN_H__
#define __MCONV_SKIN_H__


#include "mconv_node.h"


namespace mconv
{
    class Skin : public Node
    {
    public:
        Skin(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Skin()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SKIN;
        }
    };

    class SkinInfo : public Node
    {
    public:
        SkinInfo(const String &ID)
            : Node(ID)
        {

        }

        virtual ~SkinInfo()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SKIN_INFO;
        }

        Matrix4     mOffsetMatrix;
    };
}


#endif  /*__MCONV_SKIN_H__*/
