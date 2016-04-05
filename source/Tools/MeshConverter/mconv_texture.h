

#ifndef __MCONV_TEXTURE_H__
#define __MCONV_TEXTURE_H__


#include "mconv_node.h"


namespace mconv
{
    class Texture : public Node
    {
    public:
        Texture(const String &ID)
            : Node(ID)
            , mFilename("")
            , mType("diffuse")
            , mWrapModeU("repeat")
            , mWrapModeV("repeat")
        {

        }

        virtual ~Texture()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_TEXTURE;
        }

        String  mFilename;
        String  mType;
        String  mWrapModeU;
        String  mWrapModeV;
    };

    class Textures : public Node
    {
    public:
        Textures(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Textures()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_TEXTURES;
        }
    };
}


#endif  /*__MCONV_TEXTURE_H__*/
