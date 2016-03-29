

#ifndef __MCONV_MATERIAL_H__
#define __MCONV_MATERIAL_H__


#include "mconv_node.h"


namespace mconv
{
    class Material : public Node
    {
    public:
        Material(const String &ID)
            : Node(ID)
            , mMode("Lambert")
            , mShininess(20.0f)
            , mTransparency(0.0f)
            , mReflection(1.0f)
        {
            mAmbientColor[0] = mAmbientColor[1] = mAmbientColor[2] = mAmbientColor[3] = 1.0f;
            mDiffuseColor[0] = mDiffuseColor[1] = mDiffuseColor[2] = mDiffuseColor[3] = 1.0f;
            mSpecularColor[0] = mSpecularColor[1] = mSpecularColor[2] = mSpecularColor[3] = 1.0f;
            mEmissiveColor[0] = mEmissiveColor[1] = mEmissiveColor[2] = mEmissiveColor[3] = 1.0f;
        }

        virtual ~Material()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_MATERIAL;
        }

        String  mMode;
        float   mAmbientColor[4];
        float   mDiffuseColor[4];
        float   mSpecularColor[4];
        float   mEmissiveColor[4];
        float   mShininess;
        float   mTransparency;
        float   mReflection;
    };

    class Materials : public Node
    {
    public:
        Materials(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Materials()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_MATERIALS;
        }
    };
}


#endif  /*__MCONV_MATERIAL_H__*/
