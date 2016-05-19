

#include "Resource/T3DMaterial.h"
#include "Misc/T3DColor4.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"


namespace Tiny3D
{
    MaterialPtr Material::create(const String &name, MaterialType matType)
    {
        MaterialPtr material = new Material(name, matType);
        material->release();
        return material;
    }

    Material::Material(const String &name, MaterialType matType)
        : Resource(name)
        , mMaterialType(matType)
        , mAmbientColor(Color4::WHITE)
        , mDiffuseColor(Color4::WHITE)
        , mSpecularColor(Color4::WHITE)
        , mEmissiveColor(Color4::WHITE)
        , mShininess(Real(20.0))
    {
        size_t i = 0;
        for (i = 0; i < E_MAX_TEXTURE_LAYERS; ++i)
        {
            mTextureLayer[i] = nullptr;
        }
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
        bool ret = false;

        if (mMaterialType == E_MT_DEFAULT)
        {
            ArchivePtr archive;
            MemoryDataStream stream;

            if (T3D_ARCHIVE_MGR.getArchive(mName, archive))
            {
                if (archive->read(mName, stream))
                {
                    FileType fileType = parseFileType(mName);

                    switch (fileType)
                    {
                    case E_FILETYPE_UNKNOWN:
                        break;
                    case E_FILETYPE_MTB:
                        {
                            ret = loadFromBinary(stream);
                        }
                        break;
                    case E_FILETYPE_MTT:
                        {
                            ret = loadFromXML(stream);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        else
        {
            ret = true;
        }

        return true;
    }

    void Material::unload()
    {

    }

    ResourcePtr Material::clone() const
    {
        return Material::create(mName, mMaterialType);
    }

    void Material::setTexture(size_t layer, const String &name)
    {
        if (mMaterialType == E_MT_MANUAL)
        {
            mTextureLayer[layer] = T3D_TEXTURE_MGR.loadTexture(name);
        }
    }

    Material::FileType Material::parseFileType(const String &name) const
    {
        FileType fileType = E_FILETYPE_UNKNOWN;

        size_t pos = name.rfind(".");
        if (pos > 0)
        {
            String ext = name.substr(pos+1);

            if (ext == T3D_BIN_MATERIAL_FILE_EXT)
            {
                fileType = E_FILETYPE_MTB;
            }
            else if (ext == T3D_TXT_MATERIAL_FILE_EXT)
            {
                fileType = E_FILETYPE_MTT;
            }
        }

        return fileType;
    }

    bool Material::loadFromBinary(DataStream &stream)
    {
        return true;
    }

    bool Material::loadFromXML(MemoryDataStream &stream)
    {
        return true;
    }
}

