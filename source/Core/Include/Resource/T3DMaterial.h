

#ifndef __T3D_MATERIAL_H__
#define __T3D_MATERIAL_H__


#include "Resource/T3DResource.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Material : public Resource
    {
    public:
        enum MaterialType
        {
            E_MT_DEFAULT = 0,
            E_MT_MANUAL,
        };

        static MaterialPtr create(const String &name, MaterialType matType);

        virtual ~Material();

        virtual Type getType() const override;

        void setAmbientColor(const Color4 &color)
        {
            mAmbientColor = color;
        }

        const Color4 &getAmbientColor() const
        {
            return mAmbientColor;
        }

        void setDiffuseColor(const Color4 &color)
        {
            mDiffuseColor = color;
        }

        const Color4 &getDiffuseColor() const
        {
            return mDiffuseColor;
        }

        void setSpecularColor(const Color4 &color)
        {
            mSpecularColor = color;
        }

        const Color4 &getSpecularColor() const
        {
            return mSpecularColor;
        }

        void setEmissiveColor(const Color4 &color)
        {
            mEmissiveColor = color;
        }

        const Color4 &getEmissiveColor() const
        {
            return mEmissiveColor;
        }

        void setShininess(Real shininess)
        {
            mShininess = shininess;
        }

        Real getShininess() const
        {
            return mShininess;
        }

        size_t getNumTextureLayer() const
        {
            return E_MAX_TEXTURE_LAYERS;
        }

        void setTexture(size_t layer, const String &name);

        TexturePtr getTexture(size_t layer) const
        {
            T3D_ASSERT(layer < E_MAX_TEXTURE_LAYERS);
            return mTextureLayer[layer];
        }

    protected:
        Material(const String &name, MaterialType matType);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

    private:
        enum
        {
            E_MAX_TEXTURE_LAYERS = 2,
        };

        MaterialType    mMaterialType;

        Color4  mAmbientColor;
        Color4  mDiffuseColor;
        Color4  mSpecularColor;
        Color4  mEmissiveColor;

        Real    mShininess;

        TexturePtr  mTextureLayer[E_MAX_TEXTURE_LAYERS];
    };
}


#endif  /*__T3D_MATERIAL_H__*/
