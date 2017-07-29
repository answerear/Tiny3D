/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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
 ******************************************************************************/

#ifndef __MCONV_OGRE_MATERIAL_SERIALIZER_H__
#define __MCONV_OGRE_MATERIAL_SERIALIZER_H__


#include "mconv_serializer.h"
#include "mconv_ogredata.h"

#include <T3DVector3.h>
#include <T3DQuaternion.h>


namespace mconv
{
    typedef std::map<String, OgreMaterial>  OgreMaterials;
    typedef OgreMaterials::iterator         OgreMaterialsItr;
    typedef OgreMaterials::const_iterator   OgreMaterialsConstItr;
    typedef OgreMaterials::value_type       OgreMaterialsValue;

    enum MaterialScriptSection
    {
        MSS_NONE,
        MSS_MATERIAL,
        MSS_TECHNIQUE,
        MSS_PASS,
        MSS_TEXTUREUNIT,
        MSS_PROGRAM_REF,
        MSS_PROGRAM,
        MSS_DEFAULT_PARAMETERS,
        MSS_TEXTURESOURCE
    };

    struct MaterialScriptContext
    {
        MaterialScriptContext()
            : section(MSS_NONE)
            , lineNumber(0)
            , materials(nullptr)
            , material(nullptr)
            , technique(nullptr)
            , pass(nullptr)
            , textureUnit(nullptr)
        {}

        void reset()
        {
            section = MSS_NONE;
            lineNumber = 0;
            materials = nullptr;
            material = nullptr;
            technique = nullptr;
            pass = nullptr;
            textureUnit = nullptr;
        }

        MaterialScriptSection   section;
        size_t                  lineNumber;
        OgreMaterials           *materials;
        OgreMaterial            *material;
        OgreTechnique           *technique;
        OgrePass                *pass;
        OgreTextureUnit         *textureUnit;
    };

    typedef bool(*ATTRIBUTE_PARSER)(String &params, MaterialScriptContext &context);

    typedef std::map<String, ATTRIBUTE_PARSER>  AttribParserList;
    typedef AttribParserList::iterator          AttribParserListItr;
    typedef AttribParserList::const_iterator    AttribParserListConstItr;
    typedef AttribParserList::value_type        AttribParserListValue;


    class OgreMaterialSerializer : public Serializer
    {
    public:
        OgreMaterialSerializer();
        virtual ~OgreMaterialSerializer();

        virtual bool load(const String &path, void *&pData);
        virtual bool save(const String &path, void *pData);

    protected:
        bool parseScript(DataStream &stream);
        bool parseScriptLine(const String &line);

        bool invokeParser(const String &line, AttribParserList &parsers);

        MaterialScriptContext   mScriptContext;
        AttribParserList        mRootAttribParsers;
        AttribParserList        mMaterialAttribParsers;
        AttribParserList        mTechniqueAttribParsers;
        AttribParserList        mPassAttribParsers;
        AttribParserList        mTextureUnitAttribParsers;
    };
}


#endif  /*__MCONV_OGRE_MATERIAL_SERIALIZER_H__*/
