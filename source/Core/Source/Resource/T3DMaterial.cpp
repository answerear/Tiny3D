/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************************************/

#include "Resource/T3DMaterial.h"
#include "Misc/T3DColor4.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"

#include "Support/tinyxml2/tinyxml2.h"

#include <sstream>

namespace Tiny3D
{
    using namespace tinyxml2;

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
        for (size_t i = 0; i < E_MAX_TEXTURE_LAYERS; ++i)
        {
            if (mTextureLayer[i] != nullptr)
            {
                T3D_TEXTURE_MGR.unloadTexture(mTextureLayer[i]);
            }
        }
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

        return ret;
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
        mTextureLayer[layer] = T3D_TEXTURE_MGR.loadTexture(name);
    }

    void Material::setTexture(size_t layer, TexturePtr texture)
    {
        mTextureLayer[layer] = texture;
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
        bool ret = false;
        uint8_t *buffer = nullptr;
        size_t bufSize = stream.read(buffer);

        if (bufSize > 0 && buffer != nullptr)
        {
            XMLDocument *pDoc = new XMLDocument();

            if (pDoc != nullptr && pDoc->Parse((const char *)buffer, bufSize) == XML_SUCCESS)
            {
                ret = true;

                XMLDeclaration *pDeclaration = pDoc->ToDeclaration();
                XMLElement *pRootElement = pDoc->FirstChildElement(T3D_XML_TAG_TINY3D);

                if (pRootElement == nullptr)
                {
                    delete pDoc;
                    return false;
                }

                String magic = pRootElement->Attribute(T3D_XML_ATTRIB_MAGIC);
                if (magic != T3D_MATERIAL_FILE_MAGIC)
                {
                    delete pDoc;
                    return false;
                }

                String version = pRootElement->Attribute(T3D_XML_ATTRIB_VERSION);
//                 XMLElement *pVersionElement = pDoc->FirstChildElement(T3D_XML_TAG_VERSION);

//                 String version = pVersionElement->GetText();

                XMLElement *pMatElement = pRootElement->FirstChildElement(T3D_XML_TAG_MATERIAL);
                String name = pMatElement->Attribute(T3D_XML_ATTRIB_ID);

                // Mode
                XMLElement *pModeElement = pMatElement->FirstChildElement(T3D_XML_TAG_MODE);
                mMode = pModeElement->GetText();

                // Ambient
                XMLElement *pAmbientElement = pMatElement->FirstChildElement(T3D_XML_TAG_AMBIENT);
                String text = pAmbientElement->GetText();
                parseColorValue(text, mAmbientColor);

                // Diffse
                XMLElement *pDiffuseElement = pMatElement->FirstChildElement(T3D_XML_TAG_DIFFUSE);
                text = pDiffuseElement->GetText();
                parseColorValue(text, mDiffuseColor);

                // Specular
                XMLElement *pSpecularElement = pMatElement->FirstChildElement(T3D_XML_TAG_SPECULAR);
                text = pSpecularElement->GetText();
                parseColorValue(text, mSpecularColor);

                // Emissive
                XMLElement *pEmissiveElement = pMatElement->FirstChildElement(T3D_XML_TAG_EMISSIVE);
                text = pEmissiveElement->GetText();
                parseColorValue(text, mEmissiveColor);

                // Shininess
                XMLElement *pShinElement = pMatElement->FirstChildElement(T3D_XML_TAG_SHININESS);
                text = pShinElement->GetText();
                size_t start = 0;
                mShininess = getValue<Real>(text, start);

                // Transparency
                XMLElement *pTransElement = pMatElement->FirstChildElement(T3D_XML_TAG_TRANSPARENCY);
                text = pTransElement->GetText();
                start = 0;
                mTransparency = getValue<Real>(text, start);

                // Reflection
                XMLElement *pReflectElement = pMatElement->FirstChildElement(T3D_XML_TAG_REFLECTION);
                text = pReflectElement->GetText();
                start = 0;
                mReflection = getValue<Real>(text, start);

                // Number of textures
                XMLElement *pTexturesElement = pMatElement->FirstChildElement(T3D_XML_TAG_TEXTURES);
                if (pTexturesElement != nullptr)
                {
                    size_t layers = pTexturesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

                    // Texture layers
                    size_t i = 0;
                    XMLElement *pTexElement = pTexturesElement->FirstChildElement(T3D_XML_TAG_TEXTURE);

                    while (pTexElement != nullptr)
                    {
                        String texName = pTexElement->GetText();
                        mTextureLayer[i] = T3D_TEXTURE_MGR.loadTexture(texName);
                        pTexElement = pTexElement->NextSiblingElement(T3D_XML_TAG_TEXTURE);
                        i++;
                    }
                }
            }

            delete pDoc;
        }

        return ret;
    }

    void Material::parseColorValue(const String &text, Color4 &color)
    {
        size_t start = 0;
        color.red() = getValue<Real>(text, start) * 255;
        color.green() = getValue<Real>(text, start) * 255;
        color.blue() = getValue<Real>(text, start) * 255;
        color.alpha() = getValue<Real>(text, start) * 255;
    }
}

