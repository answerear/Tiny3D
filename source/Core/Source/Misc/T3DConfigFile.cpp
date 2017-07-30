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

#include "Misc/T3DConfigFile.h"
#include "Misc/T3DVariant.h"
#include "Support/tinyxml2/tinyxml2.h"

#include <T3DPlatform.h>


namespace Tiny3D
{
    #define TAG_NAME_PLIST          "plist"
    #define TAG_NAME_KEY            "key"

    #define TAG_NAME_STRING         "string"
    #define TAG_NAME_REAL           "real"
    #define TAG_NAME_INTEGER        "integer"
    #define TAG_NAME_TRUE           "true"
    #define TAG_NAME_FALSE          "false"
    #define TAG_NAME_DATE           "date"
    #define TAG_NAME_DATA           "data"
    #define TAG_NAME_ARRAY          "array"
    #define TAG_NAME_DICT           "dict"
    #define TAG_NAME_BOOL           "bool"


    ConfigFile::ConfigFile(const String &filename)
        : mFilename(filename)
    {

    }

    ConfigFile::~ConfigFile()
    {

    }

    bool ConfigFile::loadXML(Settings &settings)
    {
        bool ret = false;

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(mFilename.c_str()) == tinyxml2::XML_NO_ERROR)
        {
            ret = parseXML(doc, settings);
        }

        return ret;
    }

    bool ConfigFile::saveXML(const Settings &settings)
    {
        bool ret = false;

        tinyxml2::XMLDocument doc;
        if (ret = buildXML(doc, settings))
        {
            ret = (doc.SaveFile(mFilename.c_str()) == tinyxml2::XML_NO_ERROR);
        }

        return ret;
    }

    bool ConfigFile::loadBinary(Settings &settings)
    {
        bool ret = false;
        return ret;
    }

    bool ConfigFile::saveBinary(const Settings &settings)
    {
        bool ret = false;
        return ret;
    }

    bool ConfigFile::parseXML(const tinyxml2::XMLDocument &doc, Settings &settings)
    {
        const tinyxml2::XMLElement *plist = doc.FirstChildElement(TAG_NAME_PLIST);

        const tinyxml2::XMLElement *root = plist->FirstChildElement(TAG_NAME_DICT);

        bool ret = false;

        if (root != nullptr)
        {
            ret = parseDict(root, settings);
        }

        return ret;
    }

    bool ConfigFile::parseDict(const tinyxml2::XMLElement *root, VariantMap &dict)
    {
        bool ret = false;

        bool bKeyFound = false;
        String key;
        const tinyxml2::XMLElement *child = root->FirstChildElement();

        while (child != nullptr)
        {
            String name = child->Name();

            if (name == TAG_NAME_KEY)   // key
            {
                if (bKeyFound)
                {
                    // wrong format
                    ret = false;
                    break;
                }
                else
                {
                    // store tag value
                    key = child->GetText();
                    bKeyFound = true;
                    ret = true;
                }
            }
            else if (name == TAG_NAME_DICT) // dict
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    VariantMap subDict;
                    ret = parseDict(child, subDict);
                    if (ret)
                        dict.insert(VariantMapValue(key, subDict));
                    ret = true;
                }
                else
                {
                    // wrong format
                    ret = false;
                    break;
                }
            }
            else if (name == TAG_NAME_ARRAY)    // array
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    VariantArray subArray;
                    ret = parseArray(child, subArray);
                    if (ret)
                        dict.insert(VariantMapValue(key, subArray));
                    ret = true;
                }
                else
                {
                    // wrong format
                    ret = false;
                    break;
                }
            }
            else if (name == TAG_NAME_STRING)
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    String val = child->GetText();
                    dict.insert(VariantMapValue(key, val));
                    ret = true;
                }
                else
                {
                    ret = false;
                    break;
                }
            }
            else if (name == TAG_NAME_INTEGER)
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    long_t val = atol(child->GetText());
                    dict.insert(VariantMapValue(key, val));
                    ret = true;
                }
                else
                {
                    ret = false;
                    break;
                }
            }
            else if (name == TAG_NAME_REAL)
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    Real val = Real(atof(child->GetText()));
                    dict.insert(VariantMapValue(key, val));
                    ret = true;
                }
                else
                {
                    ret = false;
                    break;
                }
            }
            else if (name == TAG_NAME_TRUE || name == TAG_NAME_FALSE)
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    bool val = (name == TAG_NAME_TRUE);
                    dict.insert(VariantMapValue(key, val));
                    ret = true;
                }
                else
                {
                    ret = false;
                    break;
                }
            }
            else if (name == TAG_NAME_DATE)
            {
                ret = true;
            }
            else if (name == TAG_NAME_DATA)
            {
                ret = true;
            }

            child = child->NextSiblingElement();
        }

        return ret;
    }

    bool ConfigFile::parseArray(const tinyxml2::XMLElement *root, VariantArray &arr)
    {
        bool ret = false;

        String key;
        const tinyxml2::XMLElement *child = root->FirstChildElement();

        while (child != nullptr)
        {
            String name = child->Name();

            if (name == TAG_NAME_DICT)
            {
                VariantMap subDict;
                ret = parseDict(child, subDict);
                if (ret)
                    arr.push_back(subDict);
            }
            else if (name == TAG_NAME_ARRAY)
            {
                VariantArray subArray;
                ret = parseArray(child, subArray);
                if (ret)
                    arr.push_back(subArray);
            }
            else if (name == TAG_NAME_TRUE)
            {
                bool val = true;
                arr.push_back(val);
                ret = true;
            }
            else if (name == TAG_NAME_FALSE)
            {
                bool val = false;
                arr.push_back(val);
                ret = true;
            }
            else if (name == TAG_NAME_INTEGER)
            {
                long_t val = atol(child->GetText());
                arr.push_back(val);
                ret = true;
            }
            else if (name == TAG_NAME_REAL)
            {
                Real val = Real(atof(child->GetText()));
                arr.push_back(val);
                ret = true;
            }
            else if (name == TAG_NAME_STRING)
            {
                String val = child->GetText();
                arr.push_back(val);
                ret = true;
            }
            else if (name == TAG_NAME_DATE)
            {

            }
            else if (name == TAG_NAME_DATA)
            {

            }

            child = child->NextSiblingElement();
        }

        return ret;
    }

    bool ConfigFile::buildXML(tinyxml2::XMLDocument &doc, const Settings &settings)
    {
        tinyxml2::XMLDeclaration *decl = doc.NewDeclaration();
        doc.LinkEndChild(decl);

        // plist tag
        tinyxml2::XMLElement *plist = doc.NewElement(TAG_NAME_PLIST);
        doc.LinkEndChild(plist);

        // root tag
        tinyxml2::XMLElement *root = doc.NewElement(TAG_NAME_DICT);

        bool ret = buildDict(doc, root, settings);
        
        if (ret)
        {
            plist->LinkEndChild(root);
        }
        
        return ret;
    }

    bool ConfigFile::buildDict(tinyxml2::XMLDocument &doc,
        tinyxml2::XMLElement *root, const VariantMap &dict)
    {
        bool ret = false;
        VariantMapConstItr itr = dict.begin();

        while (itr != dict.end())
        {
            tinyxml2::XMLElement *child = doc.NewElement(TAG_NAME_KEY);

            const Variant &key = itr->first;
            const Variant &value = itr->second;

            tinyxml2::XMLText *text = doc.NewText(key.stringValue().c_str());
            child->LinkEndChild(text);
            root->LinkEndChild(child);

            ret = buildVariant(doc, root, value);
            if (!ret)
                break;

            ++itr;
        }

        return ret;
    }

    bool ConfigFile::buildArray(tinyxml2::XMLDocument &doc, 
        tinyxml2::XMLElement *root, const VariantArray &arr)
    {
        bool ret = false;
        tinyxml2::XMLElement *child = nullptr;
        VariantArrayConstItr itr = arr.begin();

        while (itr != arr.end())
        {
            const Variant &value = *itr;

            ret = buildVariant(doc, root, value);
            if (!ret)
                break;

            ++itr;
        }

        return ret;
    }

    bool ConfigFile::buildList(tinyxml2::XMLDocument &doc, 
        tinyxml2::XMLElement *root, const VariantList &list)
    {
        bool ret = false;
        tinyxml2::XMLElement *child = nullptr;
        VariantListConstItr itr = list.begin();

        while (itr != list.end())
        {
            const Variant &value = *itr;

            ret = buildVariant(doc, root, value);
            if (!ret)
                break;

            ++itr;
        }

        return ret;
    }

    bool ConfigFile::buildVariant(tinyxml2::XMLDocument &doc, 
        tinyxml2::XMLElement *root, const Variant &value)
    {
        bool ret = false;
        tinyxml2::XMLElement *child = nullptr;
        tinyxml2::XMLText *text = nullptr;

        switch (value.valueType())
        {
        case Variant::E_BOOL:
            {
                bool val = value.boolValue();

                if (val)
                    child = doc.NewElement(TAG_NAME_TRUE);
                else
                    child = doc.NewElement(TAG_NAME_FALSE);

                root->LinkEndChild(child);

                ret = true;
            }
            break;
        case Variant::E_INT32:
        case Variant::E_UINT32:
        case Variant::E_INT64:
        case Variant::E_UINT64:
        case Variant::E_LONG:
            {
                long_t val = value.longValue();
                child = doc.NewElement(TAG_NAME_INTEGER);
                char buf[64] = {0};
                ltoa(val, buf, 10);
                text = doc.NewText(buf);
                child->LinkEndChild(text);
                root->LinkEndChild(child);
                ret = true;
            }
            break;
        case Variant::E_REAL:
        case Variant::E_DOUBLE:
        case Variant::E_FLOAT:
            {
                Real val = value.realValue();
                child = doc.NewElement(TAG_NAME_REAL);
                char buf[64] = {0};
                snprintf(buf, sizeof(buf)-1, "%f", val);
                text = doc.NewText(buf);
                child->LinkEndChild(text);
                root->LinkEndChild(child);
                ret = true;
            }
            break;
        case Variant::E_STRING:
            {
                String val = value.stringValue();
                child = doc.NewElement(TAG_NAME_STRING);
                text = doc.NewText(val.c_str());
                child->LinkEndChild(text);
                root->LinkEndChild(child);
                ret = true;
            }
            break;
        case Variant::E_ARRAY:
            {
                const VariantArray &val = value.arrayValue();
                child = doc.NewElement(TAG_NAME_ARRAY);
                ret = buildArray(doc, child, val);
                if (ret)
                    root->LinkEndChild(child);
            }
            break;
        case Variant::E_LIST:
            {
                const VariantList &val = value.listValue();
                child = doc.NewElement(TAG_NAME_ARRAY);
                ret = buildList(doc, child, val);
                if (ret)
                    root->LinkEndChild(child);
            }
            break;
        case Variant::E_MAP:
            {
                const VariantMap &val = value.mapValue();
                child = doc.NewElement(TAG_NAME_DICT);
                ret = buildDict(doc, child, val);
                if (ret)
                    root->LinkEndChild(child);
            }
            break;
        }

        return ret;
    }
}
