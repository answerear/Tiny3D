

#include "Resource/T3DMaterial.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    MaterialPtr Material::create(const String &name)
    {
        MaterialPtr material = new Material(name);
        material->release();
        return material;
    }

    Material::Material(const String &name)
        : Resource(name)
        , mAmbientColor(Color4::WHITE)
        , mDiffuseColor(Color4::WHITE)
        , mSpecularColor(Color4::WHITE)
        , mEmissiveColor(Color4::WHITE)
        , mShininess(Real(20.0))
    {

    }

    Material::~Material()
    {

    }

    Resource::Type Material::getType() const
    {
        return E_TYPE_MATERIAL;
    }

    bool Material::load()
    {
        return true;
    }

    void Material::unload()
    {

    }

    ResourcePtr Material::clone() const
    {
        return Material::create(mName);
    }
}

