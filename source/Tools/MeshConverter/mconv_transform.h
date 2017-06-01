

#ifndef __MCONV_TRANSFORM_H__
#define __MCONV_TRANSFORM_H__


#include "mconv_node.h"


namespace mconv
{
    class Hiarachy : public Node
    {
    public:
        Hiarachy(const String &ID)
            : Node(ID)
        {
        }

        ~Hiarachy()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_HIARACHY;
        }
    };


    class Transform : public Node
    {
    public:
        Transform(const String &ID)
            : Node(ID)
            , mMesh(nullptr)
            , mSubMesh(nullptr)
        {

        }

        virtual ~Transform()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_TRANSFORM;
        }

        Matrix4     mMatrix;
        Mesh        *mMesh;
        SubMesh     *mSubMesh;
    };
}


#endif  /*__MCONV_TRANSFORM_H__*/
