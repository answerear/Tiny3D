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
    /**
     * @brief 配置文件类，用于读写配置文件，支持二进制和XML格式
     */
    class T3D_ENGINE_API ConfigFile
    {
    public:
        /** 
         * @brief 构造函数
         * @param [in] filename : 要读取的文件名，包含路径
         */
        ConfigFile(const String &filename);

        /** 
         * @brief 析构函数 
         */
        virtual ~ConfigFile();

        /**
         * @brief 从XML格式文件加载
         * @param [in][out] settings : 设置项对象
         * @return 调用成功返回T3D_ERR_OK。
         */
        TResult loadXML(Settings &settings);

        /**
         * @brief 把设置项写到XML格式文件中
         * @param [in] settings : 要保存的设置项
         * @return 调用成功返回true，否则返回false。
         */
        TResult saveXML(const Settings &settings);

        /**
         * @brief 从二进制格式文件加载
         * @param [in][out] settings : 设置项对象
         * @return 调用成功返回true，否则返回false。
         */
        bool loadBinary(Settings &settings);

        /**
         * @brief 把设置项写到二进制格式文件中
         * @param [in] settings : 要保存的设置项
         * @return 调用成功返回true，否则返回false。
         */
        bool saveBinary(const Settings &settings);

    protected:
        /** 解析 XML 格式 */
        TResult parseXML(const tinyxml2::XMLDocument &doc, Settings &settings);

        /** 解析 XML 格式字典结构容器 */
        TResult parseXMLDict(const tinyxml2::XMLElement *root, VariantMap &dict);

        /** 解析 XML 格式数组结构容器 */
        TResult parseXMLArray(const tinyxml2::XMLElement *root, VariantArray &arr);

        /** 构建 XML 格式 */
        TResult buildXML(tinyxml2::XMLDocument &doc, const Settings &settings);

        /** 从字典结构容器构建XML */
        bool buildXMLDict(tinyxml2::XMLDocument &doc, 
            tinyxml2::XMLElement *root,
            const VariantMap &dict);

        /** 从数组结构容器构建XML */
        bool buildXMLArray(tinyxml2::XMLDocument &doc, 
            tinyxml2::XMLElement *root,
            const VariantArray &arr);

        /** 根据可变类型对象构建XML */
        bool buildXMLVariant(tinyxml2::XMLDocument &doc, 
            tinyxml2::XMLElement *root,
            const Variant &value);

        /** 解析二进制格式 */
        bool parseBinary(DataStream &stream, Settings &settings);

        /** 解析二进制格式字段结构容器 */
        bool parseBinDict(DataStream &stream, VariantMap &dict);

        /** 解析二进制格式数组结构容器 */
        bool parseBinArray(DataStream &stream, VariantArray &arr);

        /** 用字典结构容器构建二进制数据 */
        bool buildBinDict(DataStream &stream, const VariantMap &dict);

        /** 用数组结构容器构建二进制数据 */
        bool buildBinArray(DataStream &stream, const VariantArray &arr);

        /** 用可变类型对象构建二进制数据 */
        bool buildBinVariant(DataStream &stream, const Variant &value);
    private:
        String  mFilename;
    };
}


#endif  /*__T3D_CONFIG_FILE_H__*/
