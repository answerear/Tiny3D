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


#include "Kernel/T3DConfigFile.h"
#include "DataStruct/T3DVariant.h"
#include "Support/tinyxml2/tinyxml2.h"
#include "T3DErrorDef.h"


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

    //--------------------------------------------------------------------------

    ConfigFile::ConfigFile(const String &filename)
        : mFilename(filename)
    {

    }

    ConfigFile::~ConfigFile()
    {

    }

    //--------------------------------------------------------------------------

    TResult ConfigFile::loadXML(Settings &settings)
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            FileDataStream fs;
            if (!fs.open(mFilename.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR("Open config file [%s] failed !", 
                    mFilename.c_str());
                break;
            }

            size_t contentSize = fs.size();
            char *content = new char[contentSize];
            if (fs.read(content, contentSize) != contentSize)
            {
                fs.close();
                ret = T3D_ERR_FILE_DATA_MISSING;
                T3D_LOG_ERROR("Read config file [%s] data failed !", 
                    mFilename.c_str());
                break;
            }

            fs.close();

            tinyxml2::XMLDocument doc;
            if (doc.Parse(content, contentSize) != tinyxml2::XML_NO_ERROR)
            {
                ret = T3D_ERR_CFG_FILE_PARSING_XML;
                T3D_LOG_ERROR("Parse xml config file [%s] failed !", 
                    mFilename.c_str());
                break;
            }

            ret = parseXML(doc, settings);
        } while (0);

        return ret;
    }

    TResult ConfigFile::parseXML(const tinyxml2::XMLDocument &doc, Settings &settings)
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            const tinyxml2::XMLElement *plist = doc.FirstChildElement(TAG_NAME_PLIST);
            if (plist == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR("Parse xml file [%s] failed ! Get xml node [plist] failed !", 
                    mFilename.c_str());
                break;
            }

            const tinyxml2::XMLElement *root = plist->FirstChildElement(TAG_NAME_DICT);
            if (root == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR("Parse xml file [%s] failed ! Get xml node [dict] failed !", 
                    mFilename.c_str());
                break;
            }

            ret = parseXMLDict(root, settings);
        } while (0);

        return ret;
    }

    TResult ConfigFile::parseXMLDict(const tinyxml2::XMLElement *root, VariantMap &dict)
    {
        TResult ret = T3D_ERR_OK;

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
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error key node !", 
                        mFilename.c_str());
                    break;
                }
                else
                {
                    // store tag value
                    key = child->GetText();
                    bKeyFound = true;
                }
            }
            else if (name == TAG_NAME_DICT) // dict
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    VariantMap subDict;
                    ret = parseXMLDict(child, subDict);
                    if (ret == T3D_ERR_OK)
                        dict.insert(VariantMapValue(key, subDict));
                    else
                        break;
                }
                else
                {
                    // wrong format
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error dict node !");
                    break;
                }
            }
            else if (name == TAG_NAME_ARRAY)    // array
            {
                if (bKeyFound)
                {
                    bKeyFound = false;
                    VariantArray subArray;
                    ret = parseXMLArray(child, subArray);
                    if (ret == T3D_ERR_OK)
                        dict.insert(VariantMapValue(key, subArray));
                    else
                        break;
                }
                else
                {
                    // wrong format
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error array node !");
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
                }
                else
                {
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error string node !");
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
                }
                else
                {
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error integer node !");
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
                }
                else
                {
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error real node !");
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
                }
                else
                {
                    ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                    T3D_LOG_ERROR("Parse xml file [%s] failed ! Error boolean node !");
                    break;
                }
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

    TResult ConfigFile::parseXMLArray(const tinyxml2::XMLElement *root, VariantArray &arr)
    {
        TResult ret = T3D_ERR_OK;

        String key;
        const tinyxml2::XMLElement *child = root->FirstChildElement();

        while (child != nullptr)
        {
            String name = child->Name();

            if (name == TAG_NAME_DICT)
            {
                VariantMap subDict;
                ret = parseXMLDict(child, subDict);
                if (ret == T3D_ERR_OK)
                    arr.push_back(subDict);
                else
                    break;
            }
            else if (name == TAG_NAME_ARRAY)
            {
                VariantArray subArray;
                ret = parseXMLArray(child, subArray);
                if (ret == T3D_ERR_OK)
                    arr.push_back(subArray);
                else
                    break;
            }
            else if (name == TAG_NAME_TRUE)
            {
                bool val = true;
                arr.push_back(val);
            }
            else if (name == TAG_NAME_FALSE)
            {
                bool val = false;
                arr.push_back(val);
            }
            else if (name == TAG_NAME_INTEGER)
            {
                long_t val = atol(child->GetText());
                arr.push_back(val);
            }
            else if (name == TAG_NAME_REAL)
            {
                Real val = Real(atof(child->GetText()));
                arr.push_back(val);
            }
            else if (name == TAG_NAME_STRING)
            {
                String val = child->GetText();
                arr.push_back(val);
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

    //--------------------------------------------------------------------------

    TResult ConfigFile::saveXML(const Settings &settings)
    {
        TResult ret = T3D_ERR_OK;
        
        do 
        {
            tinyxml2::XMLDocument doc;
            ret = buildXML(doc, settings);
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            if (doc.SaveFile(mFilename.c_str()) != tinyxml2::XML_NO_ERROR)
            {
                ret = T3D_ERR_CFG_FILE_XML_FORMAT;
                break;
            }
        } while (0);
        
        return ret;
    }
}
