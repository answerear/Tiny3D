/*******************************************************************************
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
 ******************************************************************************/

#ifndef __T3D_CONFIG_FILE_H__
#define __T3D_CONFIG_FILE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace tinyxml2
{
    class XMLDocument;
    class XMLElement;
}


namespace Tiny3D
{
    class T3D_ENGINE_API ConfigFile
    {
    public:
        ConfigFile(const String &filename);
        virtual ~ConfigFile();

        bool loadXML(Settings &settings);
        bool saveXML(const Settings &settings);

        bool loadBinary(Settings &settings);
        bool saveBinary(const Settings &settings);

    protected:
        bool parseXML(const tinyxml2::XMLDocument &doc, Settings &settings);
        bool parseDict(const tinyxml2::XMLElement *root, VariantMap &dict);
        bool parseArray(const tinyxml2::XMLElement *root, VariantArray &arr);

        bool buildXML(tinyxml2::XMLDocument &doc, const Settings &settings);
        bool buildDict(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root, 
            const VariantMap &dict);
        bool buildArray(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root, 
            const VariantArray &arr);
        bool buildList(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root,
            const VariantList &list);
        bool buildVariant(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root,
            const Variant &value);

    private:
        String  mFilename;
    };
}


#endif  /*__T3D_CONFIG_FILE_H__*/
